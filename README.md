# Monologue

## Neural Network
I have made this janky network from scratch. From the activation function, which is a simplified clone of a corvids neural activity, to the network connection.

### The Web
The Neural Network is my interpritation of a web network
* **`Connections`** : 
* **`Neuron`** : 
* **`Spiking Neuron`** : A looping output of varying signal acording to the `activation function`. This is in place to stop the network from dying out.
* **`Activation point`** : The `activation point` is the output where a Neuron connects to it and with a signal, that signal's strenght, which can combine if other neurons are connecting to the `activation point` and emmiting a signal, this strengh will be extended to the muscle it controls.
* **?** **`Repeater`** : A theorized `Item` that intake an input and will output a repeating signal of whatever was input. When an input is made into this it adopts that input and begins repeating that as an output, **?***single Output* or *`activation Function`***?**

#### Grader
* A system to help grow the network stablily.
* **`Connections`** : Takes in times it is activated and times it sucessfully overcomes the `threshhold` of whatever it is connected to. 
* **`Neuron`** : 
* **`Spiking Neuron`** :
* **`Activation point`** : 
* **Key points** : Connections must be consistantly used, and must be effective, overcoming threshholds. 

## Inner Monologue (IM)
The Inner Monologue (*IM*) is a way to keep the network from continually guessing what to do next. The *IM* is the foundation of the prospect of creating a network that can use a vocal tract simulator. This is due to my prediction that if it were to not have this feature it would repeat sounds, and try and predict from that last sound which will create a loop.

## Test:
* **`core_ai.cpp`** : This is where the network is based.
* **`Util.cpp`** : The universal file of need in your code base.

# Ideas:
* tick system
* spike and dip
* Activation nodes: a conection a Neuron can make that will activate a specific response
?Duel layer: one main layer that does the actual work, and a second tradition layer that works
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