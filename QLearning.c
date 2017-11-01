#define LEFT  1
#define RIGHT  2
#define FORWARD  0
#define GAMMA 0.5
#define TRAINED 1
#define NUM_ITERS 400
#define RANDOM_DECREASE 25
int GIVE_REWARD =  1;

/*Aliases*/
int LEFT_ENGINE = motorC;
int RIGHT_ENGINE = motorB;

/*Debugging and iteration controlling variables*/
int iteration;
float reward;
int wasRandom;
char actionString[3] = {'F', 'L', 'R'};

/*Algorythm variables*/
float Q[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
int nextState = 0;
int actualState = 0;
int lastAction = 0;
int percentageRandom = 75;

/*Sensor variables*/
float sensorActualState[3] = {0,0,0};
float sensorNextState[3] = {0,0,0};


/*Engine functions*/
	int rotateRight(int time, int power) {
		motor[RIGHT_ENGINE] = power/2;
		motor[LEFT_ENGINE] = -power/2;
		wait1Msec(time);
		motor[RIGHT_ENGINE] = 0;
		motor[LEFT_ENGINE] = 0;
		return SensorValue[S4];
	}

	int rotateLeft(int time, int power) {
		motor[RIGHT_ENGINE] = -power/2;
		motor[LEFT_ENGINE] = power/2;
		wait1Msec(time);
		motor[RIGHT_ENGINE] = 0;
		motor[LEFT_ENGINE] = 0;
		return SensorValue[S4];
	}

	int moveForward(int time, int power) {
		motor[LEFT_ENGINE] = power;
		motor[RIGHT_ENGINE] = power;
		wait1Msec(time);
		motor[RIGHT_ENGINE] = 0;
		motor[LEFT_ENGINE] = 0;
		return SensorValue[S4];
	}

/*Mathematic support functions*/
	int maxIndex(float* values) {
		int max = 0;
		if (values[1] > values[0])
			max = 1;
		if (values[2]>values[max])
			max = 2;
		return max;
	}

	float max(float* values) {
		return values[maxIndex(values)];
	}

	int minIndex(float* values) {
		int min = 0;
		if (values[1] < values[0])
			min = 1;
		if (values[2] < values[min])
			min = 2;
		return min;
	}

	float min(float* values) {
		return values[minIndex(values)];
	}

	void copyVector(float* copy, float* original, int size) {
		int i;
		for (i = 0; i < size; i++) {
			copy[i] = original[i];
		}
	}


	float treeSensorize(int time, int power){
		time = time/3;
		int sensorAcum = SensorValue[S4];

		motor[RIGHT_ENGINE] = -power/2;
		motor[LEFT_ENGINE] = power/2;
		wait1Msec(time);
		motor[RIGHT_ENGINE] = 0;
		motor[LEFT_ENGINE] = 0;
		sensorAcum+= SensorValue[S4];

		motor[RIGHT_ENGINE] = +power/2;
		motor[LEFT_ENGINE] = -power/2;
		wait1Msec(2*time);
		motor[RIGHT_ENGINE] = 0;
		motor[LEFT_ENGINE] = 0;
		sensorAcum+= SensorValue[S4];


		motor[RIGHT_ENGINE] = -power/2;
		motor[LEFT_ENGINE] = power/2;
		wait1Msec(time);
		motor[RIGHT_ENGINE] = 0;
		motor[LEFT_ENGINE] = 0;

		return sensorAcum/3;
	}


/*Algorythm functions*/
	int sensorize(float* sensorValues) {
		int time = 2000;
		int power = 25;
		sensorValues[FORWARD] = treeSensorize(time,power);
		rotateLeft(time, power);
		sensorValues[LEFT] = treeSensorize(time,power);
		rotateRight(time*2, power);
		sensorValues[RIGHT] = treeSensorize(time,power);
		rotateLeft(time,power);
		return maxIndex(sensorValues);
	}

	void act() {
		int action;
		int time = 1500;
		int power = 25;
		if (rand()%100 > percentageRandom) {
			wasRandom = 0;
			float next[3] = {0,0,0};
			next[0] = Q[actualState][0];
			next[1] = Q[actualState][1];
			next[2] = Q[actualState][2];
			action = maxIndex(next);
		} else {
			wasRandom = 1;
			action = rand()%3;
		}

		/*Direction */
		switch(action) {
			case FORWARD:
				lastAction = 0;
				break;

			case LEFT:
				rotateLeft(time,power);
				lastAction = 1;
				break;

			case RIGHT:
				rotateRight(time,power);
				lastAction = 2;
				break;
		}
		/*Movement*/
		moveForward(time, power);
	}

	float R() {
		float value = min(sensorNextState);//sensorActualState[actualState];
		if (value > 75) {
			return 0.75;
		}
		if (value > 50) {
			return value*0.01;
		}
		if (value > 25) {
			return 0;
		}
		if (value > 15) {
			return -0.5;
		}
		return -0.75;
	}

	void update() {
		float next[3] = {0,0,0};
		next[0] = Q[nextState][0];
		next[1] = Q[nextState][1];
		next[2] = Q[nextState][2];
		reward = R();
		if (GIVE_REWARD && !(reward==0))
			Q[actualState][lastAction] = reward + GAMMA*max(next);

	}

/*Debug*/
	void debug() {
			clearDebugStream();
			writeDebugStreamLine("- Iterarion %d ", iteration);
			writeDebugStreamLine("- At start, I sensed this: ");
			writeDebugStreamLine("{%d, %d, %d}", sensorActualState[0],sensorActualState[1],sensorActualState[2]);
			writeDebugStreamLine("- After moving, I sensed this: ");
			writeDebugStreamLine("{%d, %d, %d}", sensorNextState[0],sensorNextState[1],sensorNextState[2]);
			writeDebugStreamLine("- My last action was %s, and I choosed %c ", (wasRandom)?"random": "not random", actionString[lastAction]);
			writeDebugStreamLine("- My percentage to random choose an action is %d \%", percentageRandom);
			writeDebugStreamLine("- I'm advancing to state %d from state %d", actualState, nextState);
			writeDebugStreamLine("- My last reward was %f", reward);

			writeDebugStreamLine("float Q[3][3] = {");
			writeDebugStreamLine("{%f, %f, %f},", Q[0][0],Q[0][1],Q[0][2]);
			writeDebugStreamLine("{%f, %f, %f},", Q[1][0],Q[1][1],Q[1][2]);
			writeDebugStreamLine("{%f, %f, %f}", Q[2][0],Q[2][1],Q[2][2]);
			writeDebugStreamLine("};");
			writeDebugStreamLine("--------------");
	}


void setTrainedMode() {
/*Q matrix, obtained exploring*/
	float trainedQ[3][3] = {
		{1.174896, -0.876894, -0.401833},
		{-0.470270, 1.069164, -1.227539},
		{-1.170236, -1.228789, 1.164896}
	};
/*If TRAINED == 1, robot while follow only trainedQ values, without explore*/
	if (TRAINED) {
		int i = 0;
		int j = 0;

		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				Q[i][j] = trainedQ[i][j];
			}
		}
		percentageRandom = 0;
		GIVE_REWARD = 0;
	}

}

/*<<<<LACITO ROSA>>>>>*/

/*Main*/
task main()
{
	setTrainedMode(); //Disables explore mode, if needed
	/*Initializations*/
	nextState = sensorize(sensorNextState);
	copyVector(sensorActualState,sensorNextState,3);

	while(percentageRandom >= 0) {
		/*Actual(t+1) = Next(t)*/
		actualState = nextState;
		copyVector(sensorActualState,sensorNextState,3);

		/*Q-learning algorythm*/
		act();
		nextState = sensorize(sensorNextState);
		update();

		/*Debugging*/
		debug();

		/*Iteration control*/
		iteration++;
		if (iteration%NUM_ITERS == 0) {
			percentageRandom-=RANDOM_DECREASE;
		}
	}

}
