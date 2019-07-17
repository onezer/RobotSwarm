## Robot Swarm Simulator

This is a simulator to simulate multiple robots in a 2D or 3D environment.
The current robot model is full synchron with Look Compute Move phases executing at the same time.

### Look phase
The robots gather sensory information about their surroundings (currently from the direct neighbouring nodes/cells)

### Compute phase 
The robots calculate their next move

### Move phase 
Robots move

### Classes
#### Controller
Singleton class, responsible for controlling the simulation (creating, managing threads; terminating simulation etc)

#### Map
Singleton class, responsible for managing the map (environment)
Robots use its methods to look and move around the map

#### MapGenerator
Singleton class, responsible for procedurally generating the map.

#### Robot
This class represents the robots in the simulation. It's behaviour is defined by the behaviour class, that implements the iBehaviour interface. That class defines the algorithm that the robots should use.

#### iBehaviour
Every class that defines the robots' behaviour/algorithm must implement this interface.

#### iBehaviourFactory
The behaviour's factory class must implement this interface.

#### XML file generation
The simulator generates an XML file parallel to the simulation, that can be used to visualize the simulation steps in Unity.
This will help with debugging more complex robot algorithms, and visualizing 3D maps.

### Incoming features
#### Robot communication
Currently the robots have no supported communication methods, the behaviour class must implement it, with static variables/methods.

#### More swarm robot problems
Currently the only problem the simulator supports, is the filling problem, where the robots must fill every free space without colliding.
Next problem will be: resource gathering to corresponding nests.
