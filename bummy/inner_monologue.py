import numpy as np
from pathlib import Path

class InnerMonologue:
    """
    Persistent vector-based memory.
    The agent "thinks" in continuous vectors, not human language.
    """
    
    def __init__(self, path, vector_dim=256, max_length=200):
        self.path = Path(path)
        self.vector_dim = vector_dim
        self.max_length = max_length  # Max number of vectors in sequence
        
        # Ensure directory exists
        self.path.parent.mkdir(parents=True, exist_ok=True)
        
        # Initialize with near-zero random if file doesn't exist
        if not self.path.exists():
            self._init_empty()
    
    def _init_empty(self):
        """Initialize with small random vectors."""
        initial = np.random.randn(self.max_length, self.vector_dim) * 0.01
        np.save(self.path, initial)
    
    def load(self):
        """Load the current thought sequence as (max_length, vector_dim)."""
        try:
            return np.load(self.path)
        except (FileNotFoundError, ValueError):
            self._init_empty()
            return np.load(self.path)
    
    def save(self, new_thought_sequence):
        """
        Save a new thought sequence.
        Truncates or pads to max_length.
        """
        if isinstance(new_thought_sequence, np.ndarray):
            seq = new_thought_sequence
        else:
            seq = np.array(new_thought_sequence)
        
        # Ensure correct shape
        if seq.ndim == 1:
            seq = seq.reshape(1, -1)
        
        # Truncate or pad to max_length
        if len(seq) > self.max_length:
            seq = seq[-self.max_length:]  # Keep most recent
        elif len(seq) < self.max_length:
            pad = np.zeros((self.max_length - len(seq), self.vector_dim))
            seq = np.vstack([pad, seq])
        
        np.save(self.path, seq)
    
    def get_latest(self):
        """Return just the most recent thought vector."""
        return self.load()[-1]