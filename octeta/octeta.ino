// Motor constants.
int MOTOR1_PIN1 = 3;
int MOTOR1_PIN2 = 5;
int MOTOR2_PIN1 = 6;
int MOTOR2_PIN2 = 9;

// A move for our tiny robot.
struct movement {
	int mode; // Movement mode.
	int ticks; // Duration in ticks.
	int priority; // Priority (lower value = higher priority).
};

const int
	// Movement modes
	MOVE_MODE_FORWARD = 0,
	MOVE_MODE_LEFT = 1,
	MOVE_MODE_RIGHT = 2,
	MOVE_MODE_BACKWARD = 3,
	MOVE_MODE_STOP = 4,
	MOVE_MODE_SEEK_AND_DESTROY = 5,	// Rotate until you find an object, then move toward it.
	
	// Constants that can be tweaked.
	COLOR_THRESHOLD_FRONT = 600, // Maximum value for white on front sensors.
	COLOR_THRESHOLD_BACK = 750, // Maximum value for white on back sensors.
	DIST_THRESHOLD = 300, // Minimum distance for detecting an object.
	NORMAL_SPEED = 255, // Movement speed.
	ROTATE_TICKS = 200, // How many ticks in a rotation.
	DISTANCE_M = 1; // Distance multiplier.
	
// The queue of moves.
movement m[10];
	
int
	moves = 0, // Number of moves.
	ticksNoMoves = 0, // How many ticks have elapsed since we haven't made any move.
	ticksSeekAndDestroyCooldown = 0; // How many ticks are left until we can go into seek and destroy mode.
	
// Instead of directly using the value from the distance sensor, we'll use the mean of the previous 10 values, in order to eliminate noise.
const int DIST_NR = 10;
int
	lastDist[DIST_NR], // Previous values.
	lastDistCount = 0; // Number of previous values.

// Get the current movement priority.
int getCurPriority() {
	if (moves == 0) {
		return 9999;
	}
	return m[0].priority;
}

void setup() {
	pinMode(MOTOR1_PIN1, OUTPUT);
	pinMode(MOTOR1_PIN2, OUTPUT);
	pinMode(MOTOR2_PIN1, OUTPUT);
	pinMode(MOTOR2_PIN2, OUTPUT);

	Serial.begin(9600);
	
	// Use the value from the first sensor as a seed.
	randomSeed(analogRead(0));
}

// Add a new move. If there already are moves with lower priorities, delete them.
void addMove(int mode, int ticks, int priority) {
	while (
		moves > 0 &&
		m[moves - 1].priority > priority
	) {
		moves--;
	}

	m[moves].mode = mode;
	m[moves].ticks = ticks;
	m[moves].priority = priority;
	moves++;
}

void loop() {
	int
		S_FL = analogRead(0), // Color senzor; Front left.
		S_FR = analogRead(1), // Front right.
		S_BL = analogRead(3), // Back left.
		S_BR = analogRead(2), // Back right.
		S_DIST = analogRead(4); // Distance sensor.
	
	// Delete the last value and add the current one.
	int dist = 0;
	for (int i = DIST_NR - 1; i > 0; i--) {
		lastDist[i] = lastDist[i - 1];
		dist += lastDist[i];
	}
	lastDist[0] = S_DIST;
	dist += lastDist[0];
	dist /= DIST_NR; // The computed distance (mean of previous 10 values).
	
	// If there aren't 10 values yet, just use the current one directly from the sensor.
	if (lastDistCount < DIST_NR) {
		lastDistCount++;
		dist = S_DIST;
	}
	
	// Is there an object in front of us?
	bool objectDetected = dist > DIST_THRESHOLD;
		
	// Lower the cooldown.
	if (ticksSeekAndDestroyCooldown > 0) {
		ticksSeekAndDestroyCooldown--;
	}

	// Moves for two sensors.
	if (getCurPriority() > 2) {
		if (
			S_BL < COLOR_THRESHOLD_BACK &&
			S_BR < COLOR_THRESHOLD_BACK
		) {
			addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 4 * DISTANCE_M, 2);
			addMove(MOVE_MODE_RIGHT, ROTATE_TICKS * 2, 2);
			Serial.println("Spate doua;");
		}	 
		else if (
			S_FL < COLOR_THRESHOLD_FRONT &&
			S_FR < COLOR_THRESHOLD_FRONT
		) {
			addMove(MOVE_MODE_BACKWARD, ROTATE_TICKS * 4 * DISTANCE_M, 2);
			addMove(MOVE_MODE_LEFT, ROTATE_TICKS * 2, 2);
			Serial.println("Fata doua;");		
		}
		else if (
			S_FL < COLOR_THRESHOLD_FRONT &&
			S_BL < COLOR_THRESHOLD_BACK
		) {
			addMove(MOVE_MODE_RIGHT, ROTATE_TICKS, 2);
			addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 4 * DISTANCE_M, 2);
			Serial.println("Stanga doua;"); 
		}
		else if (
			S_FR < COLOR_THRESHOLD_FRONT &&
			S_BR < COLOR_THRESHOLD_BACK
		) {
			addMove(MOVE_MODE_LEFT, ROTATE_TICKS, 2);
			addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 4 * DISTANCE_M, 2);
			Serial.println("Dreapta doua;"); 
		}
	}

	// Moves for a single sensor.
	if (getCurPriority() > 5) {
		if (S_FL < COLOR_THRESHOLD_FRONT) {
			if (dist > DIST_THRESHOLD) {
				addMove(MOVE_MODE_LEFT, ROTATE_TICKS, 5);
				addMove(MOVE_MODE_BACKWARD, ROTATE_TICKS * 2 * DISTANCE_M, 5);
				Serial.println("Fata stanga; il vad in fata;");	
			}
			else {
				addMove(MOVE_MODE_BACKWARD, ROTATE_TICKS * 2 * DISTANCE_M, 5);
				addMove(MOVE_MODE_RIGHT, ROTATE_TICKS, 5);
				Serial.println("Fata stanga;");
			}
		}
		else if (S_FR < COLOR_THRESHOLD_FRONT) {
			if (dist > DIST_THRESHOLD) {
				addMove(MOVE_MODE_RIGHT, ROTATE_TICKS, 5);
				addMove(MOVE_MODE_BACKWARD, ROTATE_TICKS * 2 * DISTANCE_M, 5);
				Serial.println("Fata dreapta; il vad in fata;");
			}
			else {
				addMove(MOVE_MODE_BACKWARD, ROTATE_TICKS * 2, 5);
				addMove(MOVE_MODE_LEFT, ROTATE_TICKS, 5);
				Serial.println("Fata dreapta;");
			}
		}
		else if (S_BL < COLOR_THRESHOLD_BACK) {
			if (dist > DIST_THRESHOLD) {
				addMove(MOVE_MODE_LEFT, ROTATE_TICKS, 5);
				addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 2 * DISTANCE_M, 5);
				Serial.println("Spate stanga; il vad in fata;");
			}
			else {
				addMove(MOVE_MODE_RIGHT, ROTATE_TICKS, 5);
				addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 2 * DISTANCE_M, 5);
				Serial.println("Spate stanga;");
			}
		}
		else if (S_BR < COLOR_THRESHOLD_BACK) {
			if (dist > DIST_THRESHOLD) {
				addMove(MOVE_MODE_RIGHT, ROTATE_TICKS, 5);
				addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 2 * DISTANCE_M, 5);
				Serial.println("Spate dreapta; il vad in fata;");
			}
			else {
				addMove(MOVE_MODE_LEFT, ROTATE_TICKS, 5);
				addMove(MOVE_MODE_FORWARD,	ROTATE_TICKS * 2 * DISTANCE_M, 5);
				Serial.println("Spate dreapta;");
			}
		}
	}

	// Move!
	if (moves > 0) {
		if (getCurPriority() < 10) {
			ticksNoMoves = 0;
		}
		m[moves - 1].ticks--;

		switch(m[moves - 1].mode) {
			case MOVE_MODE_SEEK_AND_DESTROY: 
				if (!objectDetected) {					
					go(-NORMAL_SPEED, NORMAL_SPEED);
				}
				else {
					addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 16, 10);									
				}
				break;
				
			case MOVE_MODE_FORWARD:
				go(NORMAL_SPEED, NORMAL_SPEED);
				break;
	
			case MOVE_MODE_LEFT:
				go(-NORMAL_SPEED, NORMAL_SPEED);
				break;
	
			case MOVE_MODE_RIGHT:
				go(NORMAL_SPEED, -NORMAL_SPEED);
				break;
	
			case MOVE_MODE_BACKWARD:
				go(-NORMAL_SPEED, -NORMAL_SPEED);
				break;
	
			case MOVE_MODE_STOP:
				go(0, 0);
				break;
		} 

		// End the move, if the ticks reached zero.
		if (
			moves > 0 &&
			m[moves - 1].ticks == 0
		) {
			Serial.println("Am terminat un move.");
			moves--;
		}
	}
	else {
		ticksNoMoves++;

		// Too many ticks without a move? We might be head on with the enemy. Try moving back and forth a bit.
		/*
		if (ticksNoMoves > 6000) {				 
			addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
			addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 2, 7);
			addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
			addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 2, 7);
			addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
			addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 2, 7);
			addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
			addMove(MOVE_MODE_FORWARD, ROTATE_TICKS * 2, 7);
		}
		*/

		// Default move: go forth!
		go(NORMAL_SPEED, NORMAL_SPEED); 

		// Maybe do a random rotation?
		int r = random(0, 8000);
		if (r == 0) {
			addMove(MOVE_MODE_LEFT, ROTATE_TICKS * 4, 12);
			Serial.println("Random stanga.");
		} 
		else if (r == 1) {
			addMove(MOVE_MODE_RIGHT, ROTATE_TICKS * 4, 12);
			Serial.println("Random dreapta.");
		}
		else if (ticksSeekAndDestroyCooldown == 0) {
			// Or even better: seek and destroy. 
			addMove(MOVE_MODE_SEEK_AND_DESTROY, ROTATE_TICKS * 16, 11); 
			ticksSeekAndDestroyCooldown = 7000;
		}
	}	
}

// Standard method for movement.
void go(int speedLeft, int speedRight) {
	if (speedLeft > 0) {
		analogWrite(MOTOR1_PIN1, speedLeft);
		analogWrite(MOTOR1_PIN2, 0);
	}
	else {
		analogWrite(MOTOR1_PIN1, 0);
		analogWrite(MOTOR1_PIN2, -speedLeft);
	}

	if (speedRight > 0) {
		analogWrite(MOTOR2_PIN1, speedRight);
		analogWrite(MOTOR2_PIN2, 0);
	}
	else {
		analogWrite(MOTOR2_PIN1, 0);
		analogWrite(MOTOR2_PIN2, -speedRight);
	}
}