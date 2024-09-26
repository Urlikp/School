# What is snapshot?
A copy of current state of rooms. When I am inserting into next iteration, the iteration works on it, not on the original one. So, I can always go back to the original one.
# Do I want to check freed by nodes or array rooms?
## Nodes
Everytime I go to next iteration, I have to create a copy of nodes.
## Array rooms
I have to validate rooms by going through rooms[node->child->id] == 0.
This is simpler but not much changable. So if I later on discover that I need for example to know how many of boxes is somewhere, I can´t.
# Can it happen to node be valid, but I dont see it because it is a small number?
