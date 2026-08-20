# Monologue

## Neural Network
I have made this janky network from scratch. From the activation function, which is a simplified clone of a corvids neural activity, to the network connection.

### The Web
The Neural Network is my interpritation of a web network
* **`Connections`** : Connects an `Input` to an `Output`, when a value is inputed into a `Connection` the connection's weight is multiplied to the input.
* **`Neuron`** : takes in an input from one of it's input `Connections`, if this input value is over the  `Neuron`'s `threshhold` it will become active, and fire the activation function using ticks as the independent variable, where it then emits this value to it's connections.
* **`Spiking Neuron`** : A looping output of varying signal acording to the `activation function`. This is in place to stop the network from dying out.
* **`Activation point`** : The `activation point` is the output where a Neuron connects to it and with a signal, that signal's strenght, which can combine if other neurons are connecting to the `activation point` and emmiting a signal, this strengh will be extended to the muscle it controls.
* **tick system** : the time stamp of the network, each tick is a moment in which every function is run, before moving on to the next.

#### Grader
* A system to help grow the network stablily. 
* Each has a diffrent grading protocal:
    * **`Connections`** : Takes in times it is activated and times it sucessfully overcomes the `threshhold` of whatever it is connected to. 
    * **`Neuron`** : Take in the amount of sucessfull connections, which means overcoming the connected `Neuron`'s `threshhold`, additionally takes in all sucessfull acceptions, where it\`s own `threshhold` is passed. IF a `Neuron` beguins to repeadedly getting 100% of all emmitions to another `Neuron` through then the `Neuron` will cut ties with a weaker connection, a connection with less weight, and choses a random connection of the input `Neuron` and makes a weaker connection to it. If a 
    * **`Spiking Neuron`** :
    * **`Activation point`** : 
* **Key points** : Connections must be consistantly used, and must be effective, overcoming threshholds. 

## Inner Monologue (IM)
The Inner Monologue (*IM*) is a way to keep the network from continually guessing what to do next. The *IM* is the foundation of the prospect of creating a network that can use a vocal tract simulator. This is due to my prediction that if it were to not have this feature it would repeat sounds, and try and predict from that last sound which will create a loop.

## Test:
* **`core_ai.cpp`** : This is where the network is based.
* **`Util.cpp`** : The universal file of need in your code base.

# Ideas:
* spike and dip: The current activation function is just athe spike and decay, adding a dip may be benefical much later on.
* Duel layer: one main layer that does the actual work, and a second tradition layer that works
* Repeater : A theorized `Item` that intake an input and will output a repeating signal of whatever was input. When an input is made into this it adopts that input and begins repeating that as an output, **?***single Output* or *`activation Function`***?**
* Internal weights for Neurons: A weight to multiply the activation function by before emmiting it.
* Activation nodes for the vocal sim:
    * jaw open: jaw goes down
    * pull in lower lip: lower lip into mouth or teeth
    * pull in upper lip: upper lip into mouth or teeth
    * pull back lower lip: lower lip to chin
    * pull back upper lip: upper lip to nose
    * condence lips: constricting into an o shape 
    * pull left cheek: pulls left cheek back, pulling the lips to the left
    * pull right cheek pulls right cheek back, pulling the lips to the right
    * Lift tongue:
    * Lift tongue tip:
    * Drop tongue tip:
    * ? cup tongue: may just occur from Lift tongue on the roof of the mouth, Drop tongue tip ?
    * pull tongue:
    * push out tongue:
    * tongue left:
    * tongue right:
    * Tick system: