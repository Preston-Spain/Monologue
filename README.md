# Monologue

## Neural Network
I have made this janky network from scratch. From the activation function, which is a simplified clone of a corvids neural activity, to the network connection.
### The Web
The Neural Network is my interpritation of a web network
* `Connections` : 
* `Neuron` : 
* `Spiking Neuron` : A looping output of varying signal acording to the activation function. This is in place to stop the network from dying out.
* `Activation point` : The activation point is the output where a Neuron connects to it and with a signal, that signal's strenght, which can combine if other neurons are connecting to the activation point and emmiting a signal, this strengh will be extended to the muscle it controls.

## Inner Monologue (IM)
The Inner Monologue (IM) is a way to keep the network from continually guessing what to do next. The IM is the foundation of the prospect of creating a network that can use a vocal tract simulator. This is due to my prediction that if it were to not have this feature it would repeat sounds, and try and predict from that last sound which will create a loop.

## Test:
* `core_ai.cpp`: This is where the network is based.
* `Util.cpp`: The universal file of need in your code base.