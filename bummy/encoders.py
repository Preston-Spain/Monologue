import torch
import torchvision.transforms as transforms
from torchvision.models import mobilenet_v2, MobileNet_V2_Weights
import numpy as np

class VisionEncoder:
    """Converts screen pixels to a semantic feature vector."""
    
    def __init__(self):
        # Load pre-trained MobileNetV2 (no classifier head)
        self.model = mobilenet_v2(weights=MobileNet_V2_Weights.DEFAULT)
        self.model.classifier = torch.nn.Identity()  # Remove classifier
        self.model.eval()
        
        # Preprocessing pipeline
        self.transform = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.485, 0.456, 0.406],
                               std=[0.229, 0.224, 0.225])
        ])
        
    def encode(self, image_np):
        """
        image_np: (224, 224, 3) numpy array in [0,1]
        Returns: (1280,) feature vector
        """
        # Convert to tensor and preprocess
        tensor = self.transform(image_np).unsqueeze(0)
        
        with torch.no_grad():
            features = self.model(tensor)
        
        return features.squeeze().cpu().numpy()

import librosa
import numpy as np

class AudioEncoder:
    """Converts raw audio to MFCC feature vector."""
    
    def __init__(self, sample_rate=16000, n_mfcc=13):
        self.sample_rate = sample_rate
        self.n_mfcc = n_mfcc
        
    def encode(self, audio_chunk):
        """
        audio_chunk: 1D numpy array of raw audio
        Returns: (n_mfcc,) feature vector (averaged over time)
        """
        mfcc = librosa.feature.mfcc(
            y=audio_chunk,
            sr=self.sample_rate,
            n_mfcc=self.n_mfcc
        )
        # Average across time dimension
        return np.mean(mfcc, axis=1)