#!/usr/bin/env python3
"""
Main orchestration loop for the AI agent.
Runs on Raspberry Pi 5 (8GB).
"""
import time
import numpy as np
from datetime import datetime
from pathlib import Path

# Import all subsystems
from sensors import ScreenCapture, AudioCapture
from encoders import VisionEncoder, AudioEncoder
from core_ai import StateStreamTransformer
from inner_monologue import InnerMonologue
from monitor import Monitor
from actuators import SpeechActuator, CursorActuator

# Configuration
CYCLE_TARGET_MS = 150  # Target cycle time (6-7 Hz "thought" rate)
IM_PATH = Path("/home/pi/agent_data/current_thought.npy")

class Agent:
    def __init__(self):
        print("Initializing Agent subsystems...")
        self.screen = ScreenCapture()
        self.mic = AudioCapture()
        self.vision_enc = VisionEncoder()
        self.audio_enc = AudioEncoder()
        self.im = InnerMonologue(IM_PATH, max_length=200)
        self.brain = StateStreamTransformer()
        self.monitor = Monitor()
        self.speech = SpeechActuator()  # Sends to Speech Pi via ZeroMQ
        self.cursor = CursorActuator()
        self.cycle_count = 0
        
    def run(self):
        print("Agent is now alive. Entering main loop.")
        while True:
            cycle_start = time.perf_counter()
            self.cycle_count += 1
            
            # 1. SENSE - Capture raw data
            screen_rgb = self.screen.capture()
            audio_chunk = self.mic.capture()
            
            # 2. ENCODE - Convert to features
            vision_features = self.vision_enc.encode(screen_rgb)
            audio_features = self.audio_enc.encode(audio_chunk)
            
            # 3. RETRIEVE - Load current Inner Monologue
            im_vector = self.im.load()
            
            # 4. THINK - Run the Core AI
            new_im, speech_params, action_params, prediction_error = self.brain.forward(
                vision_features, audio_features, im_vector
            )
            
            # 5. REGULATE - Monitor checks for loops, overheating, etc.
            regulated_im, slowdown, should_reset = self.monitor.regulate(
                new_im, prediction_error, cycle_time_ms
            )
            
            if should_reset:
                regulated_im = np.random.randn(256) * 0.01  # Reset to near-zero
            
            # 6. REMEMBER - Save the new IM state
            self.im.save(regulated_im)
            
            # 7. ACT - Send outputs
            self.speech.speak(speech_params)
            self.cursor.move(action_params)
            
            # 8. CYCLE TIMING - Enforce consistent "thought" speed
            cycle_end = time.perf_counter()
            elapsed_ms = (cycle_end - cycle_start) * 1000
            if elapsed_ms < CYCLE_TARGET_MS:
                time.sleep((CYCLE_TARGET_MS - elapsed_ms) / 1000.0)
            
            # Log every 100 cycles
            if self.cycle_count % 100 == 0:
                print(f"[{self.cycle_count}] IM mean: {np.mean(regulated_im):.3f}, "
                      f"Error: {prediction_error:.3f}, Cycle: {elapsed_ms:.1f}ms")

if __name__ == "__main__":
    agent = Agent()
    agent.run()