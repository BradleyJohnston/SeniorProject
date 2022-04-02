#ifndef CONVOLUTIONAL_NEURAL_NETWORK
#define CONVOLUTIONAL_NEURAL_NETWORK

class ConvolutionalNeuralNetwork
{
private:
	// Input
	float**** input;
	int numberOfInputs;
	int inputHeight;
	int inputWidth;
	int inputChannels;

	// Activation Map
	float**** activationMap;
	int featureMapHeight;
	int featureMapWidth;
	int featureMapChannels;

	
};

#endif // CONVOLUTIONAL_NEURAL_NETWORK