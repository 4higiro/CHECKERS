#ifndef TREE
#define TREE

template <typename T>
class tree_node
{
private:
	tree_node<T>** subs;
	int i;
	int n;
public:
	T element;

	tree_node(T element, int n = 0)
	{
		this->element = element;
		this->n = n;
		subs = new tree_node*[n];
		i = 0;
	}

	int size()
	{
		return i;
	}

	int capacity()
	{
		return n;
	}

	tree_node<T>* getSub(int j)
	{
		return subs[j];
	}

	void add(T element, int n = 0)
	{
		if (i < this->n)
		{
			subs[i] = new tree_node(element, n);
			i++;
		}
	}

	static void clear(tree_node<T>* node)
	{
		if (node->i != 0)
		{
			for (int j = 0; j < node->i; j++)
				clear(node->subs[j]);
			delete[] node->subs;
			node->i = 0;
			node->element = T();
		}
	}

	void push()
	{
		clear(subs[--i]);
	}
};

#endif
