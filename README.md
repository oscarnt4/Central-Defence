# Central-Defence

2D Tower Defence game in C++

 Explainer Video

 [![Central Defence Explained](https://img.youtube.com/vi/7UC6AMXg7TQ/0.jpg)](https://www.youtube.com/watch?v=7UC6AMXg7TQ)

Central Defence is a tower defence-style game where the player must protect a central circular base from incoming projectiles. The player is represented by an object which rotates around the central base at a fixed speed. The player can switch the direction of rotation between clockwise and anticlockwise using the left and right arrow keys. The player may also modify their distance from the base by increasing or decreasing their radius of rotation with the up and down arrow keys. 

Input	     ->      Action:

Left Arrow ← 	 ->  Change rotation direction to anticlockwise.

Right Arrow → 	 -> Change rotation direction to clockwise.

Up Arrow ↑ 	  ->   Increase the radius of rotation about the base.

Down Arrow ↓ 	 ->  Decrease the radius of rotation about the base.

These inputs allow the player to traverse almost the entire game window (except for the corners) allowing the player to reach any projectile before it comes into contact with the base. They also limit the player by forcing them to only move in a rotating pattern and only at a specific speed.

Projectiles spawn at regular intervals around the game window and move towards the base at the same speed. If a projectile reaches the base, it deals damage, gradually reducing the base’s health. However, if the player manages to intercept a projectile first, it is destroyed, preventing any damage to the base. If the base’s health be reduced to zero, the game ends and resets and the player must start again from the beginning.

The game is designed to be endless, with difficulty progressively increasing as each level is completed. As the player advances through the levels, projectiles spawn more frequently. The challenge is to survive for as long as possible by defending the base from these increasingly abundant waves of projectiles.

Once per level a powerup will spawn in place of a projectile. These powerups could be one of two types: a speed powerup or a size powerup. The size powerup increases the size of the player’s rotating object, making it easier to collide with and destroy incoming projectiles. The speed powerup boosts the player’s rotation speed, giving the player a better chance of reaching projectiles before they come into contact with the base. Powerups move slower, making it harder for the player to time their collection well as well as adding a layer of tactical decision-making for the best moment to collect them. Below is a key which shows the colours of all the game objects and what those colours mean:

Object Colour	  ->  Object Type:

White      ->       Base

Cyan       ->       Player

Red         ->      Projectile

Green      ->       Speed powerup

Magenta     ->      Size powerup

Central Defence does not have an endpoint or win condition. Instead, it continuously increases in difficulty until the player can no longer keep up with the pace of the projectiles. This design choice ensures that the game is challenging and frustrating, allowing for plenty of room for the player to improve.
