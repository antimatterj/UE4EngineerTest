# UE4EngineerTest
This project currently runs on UE4 4.24.3

----------------------------------------------
## Instructions
### Setup

Fork this repository before cloning.
Once forked, feel free to submit to it as much as you'd like.
When completed, send us an email and a link to your fork of the repository.

### Create a robot that randomly navigates an environment.

- The pawn should move forward from its starting location anywhere in the map by 10 units at a time until it 'hits' something (until an object is less than 15 units away from it in its forward direction), then rotate left or right a little (randomly, between 5 and 25 degrees), and try moving forward again, repeating the process indefinitely
- The pawn's starting location should be 10 units above the floor; you don't have to enforce this in code, just drop it in and move it up by 10 units
- Rotating and moving forward can happen discretely and don't need to be animated (i.e. move 10 units or rotate n degrees all at once, with every frame representing a new rotation or position)
- In between moves, wait for one second before proceeding to the next move. Where and how this is implemented (C++, BP node graph) is up to you.
- You can assume that the starting Z location of the pawn will always be its Z location (no downward line checks and floor placement necessary)

### Have the robot take pictures and output information

- After every time the pawn moves forward or rotates, take a screenshot and save it to the saved folder.
- In addition to the screenshot, output a text file with a list of the actor names that are in the viewport when the image is taken
  - Each image should have a corresponding text file. ie: image_1.png and image_1_actors.txt

The collision check against objects and rotation of the pawn should happen on the C++ side. The easiest way to set this up would likely be a C++ Blueprint with the C++ functionality handling line traces for obstruction checks and choice of new direction called from the blueprint's node graph during tick. Please use UE4 conventions and frameworks for C++ implementation.
Online research, UE4 documentation, Stackoverflow, etc., are all fair game. Google to your heart's content :)

When hitting Play In Editor, the pawn should start navigating and dropping spheres into the environment.
We have created a map in the project called `AIReverieMap.umap` in the `AIReverie` folder. Feel free to use that map or create your own.
If you create your own, ensure it has a variety of objects in it.

A game mode has also been created and set as the default in the project, which can be found at `AIReverie/BP_AIReverie_GameMode`

If you have any questions at all please email whoever sent this to you. We are more than happy to answer any questions.
