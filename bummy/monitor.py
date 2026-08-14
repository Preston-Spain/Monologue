import time
import psutil
import numpy as np
from collections import deque

class Monitor:
    """
    Regulates the agent's inner state.
    Prevents: runaway loops, overheating, repetition, semantic stagnation.
    """
    
    def __init__(self):
        # Homeostatic limits
        self.max_im_length = 200
        self.min_im_length = 10
        self.max_cycle_time = 2000  # ms
        self.min_cycle_time = 10    # ms
        self.max_cpu = 80           # percent
        self.max_temp = 70          # Celsius
        
        # Repetition detection (semantic, not exact)
        self.history = deque(maxlen=20)
        self.similarity_threshold = 0.85
        
        # State
        self.slowdown_factor = 1.0
        self.consecutive_repeats = 0
        self.last_im = None
    
    def regulate(self, im_vector, prediction_error, cycle_time_ms):
        """
        im_vector: new IM vector from Core AI
        prediction_error: how surprised the AI was (0-1)
        cycle_time_ms: how long this cycle took
        
        Returns: (regulated_im, slowdown_factor, should_reset)
        """
        # 1. Semantic repetition detection
        if self.last_im is not None:
            similarity = np.dot(im_vector, self.last_im) / (
                np.linalg.norm(im_vector) * np.linalg.norm(self.last_im) + 1e-8
            )
            if similarity > self.similarity_threshold:
                self.consecutive_repeats += 1
            else:
                self.consecutive_repeats = max(0, self.consecutive_repeats - 1)
        else:
            self.consecutive_repeats = 0
        
        self.last_im = im_vector.copy()
        self.history.append(im_vector)
        
        # 2. If stuck in semantic loop, inject novelty
        if self.consecutive_repeats > 5:
            # Add random noise to break the loop
            im_vector = im_vector + np.random.randn(len(im_vector)) * 0.1
            self.consecutive_repeats = 0
        
        # 3. CPU/Temperature throttling
        cpu = psutil.cpu_percent()
        temp = self._get_cpu_temp()
        
        if cpu > self.max_cpu or temp > self.max_temp:
            self.slowdown_factor = min(self.slowdown_factor * 1.5, 5.0)
        elif prediction_error < 0.1 and len(self.history) > 10:
            self.slowdown_factor = max(self.slowdown_factor * 0.9, 0.5)
        
        # 4. Cycle time enforcement
        if cycle_time_ms > self.max_cycle_time:
            # Too slow - might be stuck, force a reset
            should_reset = True
        elif cycle_time_ms < self.min_cycle_time:
            # Too fast - add delay
            time.sleep((self.min_cycle_time - cycle_time_ms) / 1000.0)
            should_reset = False
        else:
            should_reset = False
        
        # 5. Emergency reset if prediction error is maxed out
        if prediction_error > 0.95:
            should_reset = True
        
        return im_vector, self.slowdown_factor, should_reset
    
    def _get_cpu_temp(self):
        """Read Raspberry Pi CPU temperature."""
        try:
            with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
                return float(f.read()) / 1000.0
        except (FileNotFoundError, OSError):
            return 40.0  # Fallback