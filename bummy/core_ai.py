import torch
import torch.nn as nn
import numpy as np

class StateStreamTransformer(nn.Module):
    """
    A lightweight transformer with persistent state.
    This is the "brain" that generates thoughts from sensory input.
    """
    
    def __init__(self, vision_dim=1280, audio_dim=13, im_dim=256, hidden_dim=512, num_heads=8):
        super().__init__()
        self.hidden_dim = hidden_dim
        self.im_dim = im_dim
        
        # Input projection
        self.input_proj = nn.Linear(vision_dim + audio_dim + im_dim, hidden_dim)
        
        # Transformer encoder (stateful via persistent KV cache)
        self.transformer = nn.TransformerEncoder(
            nn.TransformerEncoderLayer(
                d_model=hidden_dim,
                nhead=num_heads,
                dim_feedforward=hidden_dim * 4,
                dropout=0.1,
                batch_first=True
            ),
            num_layers=4
        )
        
        # Output heads
        self.im_head = nn.Linear(hidden_dim, im_dim)
        self.speech_head = nn.Linear(hidden_dim, 64)  # Speech parameters
        self.action_head = nn.Linear(hidden_dim, 8)   # Cursor/actions
        self.error_head = nn.Linear(hidden_dim, 1)    # Prediction error
        
        # State persistence (KV cache would be here in full implementation)
        self.state = None
    
    def forward(self, vision_feat, audio_feat, im_seq):
        """
        vision_feat: (vision_dim,) numpy array
        audio_feat: (audio_dim,) numpy array
        im_seq: (max_len, im_dim) numpy array - the current IM
        
        Returns: (new_im, speech_params, action_params, prediction_error)
        """
        # Convert to tensors
        v = torch.from_numpy(vision_feat).float().unsqueeze(0)  # (1, vision_dim)
        a = torch.from_numpy(audio_feat).float().unsqueeze(0)   # (1, audio_dim)
        im = torch.from_numpy(im_seq).float().unsqueeze(0)      # (1, seq_len, im_dim)
        
        # Flatten IM sequence for input
        im_flat = im.mean(dim=1)  # (1, im_dim) - simple pooling
        
        # Concatenate all inputs
        combined = torch.cat([v, a, im_flat], dim=1)  # (1, vision+audio+im)
        combined = self.input_proj(combined).unsqueeze(1)  # (1, 1, hidden_dim)
        
        # Pass through transformer
        output = self.transformer(combined)  # (1, 1, hidden_dim)
        out = output.squeeze(1)  # (1, hidden_dim)
        
        # Generate outputs
        new_im = self.im_head(out).detach().numpy().flatten()
        speech = self.speech_head(out).detach().numpy().flatten()
        action = self.action_head(out).detach().numpy().flatten()
        error = torch.sigmoid(self.error_head(out)).item()
        
        return new_im, speech, action, error
    
    def step(self, *args):
        """Alias for forward() to match main loop naming."""
        return self.forward(*args)