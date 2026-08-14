#!/usr/bin/env python3
"""
Speech Node - Runs on Raspberry Pi 3/4.
Receives speech parameters from Main Pi via ZeroMQ and synthesizes audio.
"""

import zmq
import json
import subprocess
import tempfile
import os
from pathlib import Path

class SpeechNode:
    def __init__(self, port=5555, voice_model="/home/pi/piper_voices/en_US-amy-medium.onnx"):
        self.voice_model = voice_model
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.PULL)
        self.socket.bind(f"tcp://*:{port}")
        print(f"Speech Node listening on port {port}")
        
    def run(self):
        while True:
            # Wait for message from Main Pi
            message = self.socket.recv_json()
            params = message.get("params", [])
            
            # Convert parameters to text (in real system, this would be
            # articulatory-to-phoneme mapping. For now, use a placeholder.)
            text = self._params_to_text(params)
            
            if text:
                self._speak(text)
    
    def _params_to_text(self, params):
        """
        Placeholder: Convert articulatory parameters to text.
        In a full implementation, this would be a neural decoder.
        """
        # Simplified: just echo a test phrase
        return "Hello, I am thinking."
    
    def _speak(self, text):
        """Synthesize speech using Piper TTS."""
        with tempfile.NamedTemporaryFile(suffix=".wav", delete=False) as tmp:
            output_path = tmp.name
        
        try:
            # Run Piper TTS
            cmd = [
                "piper",
                "--model", self.voice_model,
                "--output_file", output_path,
                "--input", text
            ]
            subprocess.run(cmd, check=True, capture_output=True)
            
            # Play the audio (using aplay on Pi)
            subprocess.run(["aplay", output_path], check=True)
            
        except subprocess.CalledProcessError as e:
            print(f"TTS Error: {e.stderr}")
        finally:
            # Clean up temp file
            if os.path.exists(output_path):
                os.unlink(output_path)

if __name__ == "__main__":
    node = SpeechNode()
    node.run()