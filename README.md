# Monologue

Pi set up


On the Main Pi:
``` bash
pip install pyzmq
```

On the Speech Pi:
``` bash
# Install Piper TTS
pip install piper-tts

# Download a voice model
python -m piper.download_voices en_US-amy-medium

# Install ZeroMQ
sudo apt-get install libzmq3-dev
pip install pyzmq
```

* list of requirements:
- torch
- torchvision
- numpy
- mss
- Pillow
- sounddevice
- librosa
- pyzmq
- psutil
- pyautogui

can be installed in `requirements.txt`
run to instal:
``` bash
pip install -r requirements.txt
```