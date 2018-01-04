Robotic Q learning in C
============

Q-learning is a model-free reinforcement learning technique, which is used to make a two-wheeled simulated robot learn how to reach the other side of twisted hallways. This was a project for the robotics course in my senior year of my undergrad in computer science at UDC (Spain) and is implemented in C. 


## Problem explanation

We have a robot with 2 movement-independent wheels and we have to train a behavior matrix to find the optimal strategy to reach the other side of a hallway filled with twist. The only sensor the robot is equipped with is a distance meter that measures the distance to where the robot is pointing.


## Strategy
As we only have directional measurements of distance. At each step, the robot will rotate and gather measures from the front, 90º to the left and 90º to the right. This will help construct the 3x3 matrix of 3 possible actions (move straight, rotate left and move and rotate right and move) and the 3 possible states (the maximum distance is front, the maximum distance is left, the maximum distance is right). The robot will act based on the matrix and the feedback function will reward the decision as a function of the minimum distance to a wall and learning parameter gamma.
After the training, the optimal matrix will tell the robot to move to where the distance to a wall is maximum.

## Contact

Contact [Daniel Ruiz Perez](mailto:druiz072@fiu.edu) for requests, bug reports and good jokes.


## License

The software in this repository is available under the GNU General Public License, version 3. See the [LICENSE](https://github.com/DaniRuizPerez/PyGame/blob/master/LICENSE) file for more information.
