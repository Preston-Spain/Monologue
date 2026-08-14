import zmq
import json
import numpy as np

class SpeechActuator:
    """
    Sends speech parameters to the Speech Node (Pi 3/4).
    Uses ZeroMQ for reliable, low-latency communication.
    """
    
    def __init__(self, host="192.168.1.xxx", port=5555):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.PUSH)
        self.socket.connect(f"tcp://{host}:{port}")
        
    def speak(self, speech_params):
        """
        speech_params: numpy array of articulatory parameters
        """
        # Convert to JSON-serializable list
        data = {
            "params": speech_params.tolist(),
            "timestamp": time.time()
        }
        self.socket.send_json(data)

import pyautogui
import numpy as np

class CursorActuator:
    """Moves the mouse cursor based on action parameters."""
    
    def __init__(self, screen_width=1920, screen_height=1080):
        self.width = screen_width
        self.height = screen_height
        
    def move(self, action_params):
        """
        action_params: [dx, dy, click] 
        dx, dy: normalized movement (-1 to 1)
        click: 0 or 1
        """
        if len(action_params) < 3:
            return
        
        dx, dy, click = action_params[0], action_params[1], action_params[2]
        
        # Scale to screen coordinates
        move_x = dx * 50  # 50 pixels per normalized unit
        move_y = dy * 50
        
        pyautogui.moveRel(move_x, move_y)
        
        if click > 0.5:
            pyautogui.click()