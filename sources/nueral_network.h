#ifndef NN
#define NN

#include <random>

class nueral_network
{
private:
	float w1, w2, b;

	float getRand()
	{
		return (rand() % 51) / 10.0f;
	}
public:
	nueral_network()
	{
		w1 = getRand();
		w2 = getRand();
		b = getRand();
	}

	nueral_network(float w1, float w2, float b)
	{
		this->w1 = w1;
		this->w2 = w2;
		this->b = b;
	}

	float getRate(int x, int y) const
	{
		return x * w1 + y * w2 + b;
	}

	void getConfig(float& w1, float& w2, float& b) const
	{
		w1 = this->w1;
		w2 = this->w2;
		b = this->b;
	}
};

#endif
