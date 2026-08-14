import mss
import numpy as np
from PIL import Image

class ScreenCapture:
    """Captures the agent's screen (its primary visual world)."""
    
    def __init__(self, monitor=1, target_size=(224, 224)):
        self.sct = mss.mss()
        self.monitor = self.sct.monitors[monitor]  # Primary display
        self.target_size = target_size
        
    def capture(self):
        """Returns a numpy array (H, W, 3) in RGB."""
        # Capture raw screenshot
        screenshot = self.sct.grab(self.monitor)
        # Convert to PIL Image then numpy
        img = Image.frombytes("RGB", screenshot.size, screenshot.bgra, "raw", "BGRX")
        img = img.resize(self.target_size, Image.Resampling.LANCZOS)
        return np.array(img).astype(np.float32) / 255.0  # Normalize to [0,1]

import sounddevice as sd
import numpy as np
import librosa

class AudioCapture:
    """Captures microphone input and extracts features."""
    
    def __init__(self, sample_rate=16000, duration=0.5, n_mfcc=13):
        self.sample_rate = sample_rate
        self.duration = duration
        self.n_mfcc = n_mfcc
        
    def capture(self):
        """Returns raw audio chunk (1D numpy array)."""
        recording = sd.rec(
            int(self.sample_rate * self.duration),
            samplerate=self.sample_rate,
            channels=1,
            dtype='float32'
        )
        sd.wait()
        return recording.flatten()