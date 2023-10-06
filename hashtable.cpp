/**
This code defines a hash table data structure using open addressing and cuckoo hashing with a variable number of bits for the hash function. The hash 
table is implemented using buckets, and each bucket contains a vector of key-value pairs. If the bucket overflows, the keys are rehashed and inserted into a new bucket.
**/

#include<iostream>
#include<vector>
using namespace std;
template<typename Key>
class Bucket {
	vector <Key> values;
	Bucket* overflow;
	int size;
public:

	Bucket(int size) {
		this->size = size;
		overflow = NULL;
	}
	bool isEmpty() {
		return (values.size() == 0);
	}

	bool isOverflow() {
		return (overflow != NULL);
	}

	void copy(vector <Key>& temp) {
		for (int i = 0; i < values.size(); i++)
			temp.push_back(values[i]);
		values.clear();
		if (overflow) {
			overflow->copy(temp);
			delete(overflow);
			overflow = NULL;
		}
	}

	int del(Key key) {
		for (unsigned int i = 0; i < values.size(); i++) {
			if (values[i] == key) {
				values.erase(values.begin() + i);
				if (overflow) {
					values.push_back(overflow->values[0]);
					int status = overflow->del(overflow->values[0]);
					if (overflow->isEmpty()) {
						delete(overflow);
						overflow = NULL;
					}
				}
				return 1;
			}
		}
		if (overflow) {
			int status = overflow->del(key);
			if (overflow->isEmpty()) {
				delete(overflow);
				overflow = NULL;
			}
			return status;
		}
		else
			return 0;
	}

	void display() {
		for (int i = 0; i < values.size(); i++)
			cout << values[i] << " ";
		if (overflow)
			overflow->display();
		else
			cout << endl;
	}

	void insert(Key key) {
		if (values.size() < size)
			values.push_back(key);
		else {
			if (overflow == NULL)
				overflow = new Bucket(size);
			overflow->insert(key);
		}

	}

	

	int search(Key key) {
		for (int i = 0; i < values.size(); i++)
			if (values[i] == key)
				return 1;
			else if (overflow)
				return overflow->search(key);
		return 0;
	}
};

template<typename k, size_t N>
class ADS_set {
	int numrecords;
	int next;
	int numbits=N;
	int bucketsize;
	vector<Bucket<k>*> buckets;
public:
	class Iterator;
	using value_type = k;
	using key_type = k;
	using reference = key_type&;
	using const_reference = const key_type&;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using iterator = Iterator;
	using const_iterator = Iterator;
	using key_compare = std::less<key_type>;   
	using key_equal = std::equal_to<key_type>; 
	using hasher = std::hash<key_type>;      


	//Contructor that always be invoked 
	ADS_set(int bucketsize) {
		this->bucketsize = bucketsize;
		next = 0;
		numrecords = 1 << numbits;
		for (int i = 0; i < numrecords; i++)
			buckets.push_back(new Bucket<k>(bucketsize));
	}

	//Hash functions
	int hash(k n) {
		return n & ((1 << numbits) - 1);
	}

	void reinsert(vector <k>& temp) {
		for (unsigned int i = 0; i < temp.size(); i++) {
			k key = temp[i];
			int num = hash(key);
			buckets[num]->insert(key);
		}
	}

	//main Function to insert
	void insert(key_type key) {
		if (!count(key)) { // this will check and don't insert any key again

			int num = hash(key);
			if (num >= buckets.size()) {
				num = num - (1 << (numbits - 1));
			}
			buckets[num]->insert(key);
			if (!buckets[num]->isOverflow())
				return;
			buckets.push_back(new Bucket<k>(bucketsize));
			numbits = ceil(log2((double)buckets.size()));
			vector<k> temp;
			buckets[next]->copy(temp);
			reinsert(temp);
			next++;
			if (next == 1 << (numbits - 1)) {
				next = 0;
			}
		}
	}

	//Inserting values by using initializer list 
	//Overloaded function of insert
	void insert(initializer_list<key_type> ilist) {

		for (auto it = ilist.begin(); it != ilist.end(); it++) {

			if (!count(it)) {
				this->insert(it);
			}
		}
	}

	//Display
	void display() {
		cout << "INDEX OF NEXT BUCKET TO BE SPLIT " << next << endl;
		for (int i = 0; i < buckets.size(); i++) {
			cout << i << " ";
			buckets[i]->display();
		}
	}

	//return bool if key exists
	bool search(key_type key) {
		unsigned int num = hash(key);
		if (num < buckets.size()) {
			int status = buckets[num]->search(key);
			if (status)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else {
			num -= 1 << (numbits - 1);
			int status = buckets[num]->search(key);
			if (status)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	//Return refrence of the bucket
	//overloaded function
	Bucket<key_type>* searchh(const key_type& key) {

		int num = hash(key);
		if (num < buckets.size()) {
			int status = buckets[num]->search(key);
			if (status)
			{
				return buckets[num];
			}
			else
			{
				return NULL;
			}
		}
		else {
			num -= 1 << (numbits - 1);
			int status = buckets[num]->search(key);
			if (status)
			{
				return buckets[num];
			}
			else
			{
				return NULL;
			}
		}
	}

	//This will delete key from the function
	void del(k key) {
		unsigned int num = hash(key);
		if (num < buckets.size()) {
			int status = buckets[num]->del(key);
			if (status)
				cout << key << " deleted from bucket index " << num << endl;
			else
				cout << key << " not found" << endl;
		}
		else {
			num -= 1 << (numbits - 1);
			int status = buckets[num]->del(key);
			if (status)
				cout << key << " deleted from bucket index " << num << endl;
			else
				cout << key << " not found" << endl;
		}
	}

	////////////////_REQUIRED CONSTRUCTORS_//////////////////////////////////////

	//Default Container Creation
	ADS_set() :ADS_set(N)
	{

	}

	//Create A Container Containing Elements From ilist
	ADS_set(initializer_list<key_type> ilist): ADS_set(N) {

		insert(ilist);
	}

	//Range Constructor
	template<typename Inputit> ADS_set(Inputit begin,Inputit end):ADS_set (N)
	{
		for (auto it = begin; it != end; ++it) {
			this->insert(*it);
		}
	}

	//Copy Constructor
	ADS_set(const ADS_set& other) : ADS_set(N) {

		for (const auto& key : other) {
			this->insert(key);
		}

	}

	////////////////_REQUIRED METHODS_//////////////////////////////////////

	void clear()
	{
		this->buckets.clear();
	}

	//Clear the existing values from the table,
	//copy values from the current object to the existing one
	//return refrence of current object
	ADS_set& operator= (const ADS_set& other) {
		clear();

		for (const auto& key : other)
		{
			insert(key);
		}

		return *this;
	}

	//Clear the existing values from the table,
	//copy values from the initializer to the existing one
	//return refrence of current object
	ADS_set& operator= (std::initializer_list<key_type> ilist) {
		clear();
		isnert(ilist);
		return *this;
	}

	//Returns size
	size_type size() const {

		return buckets.size();
	}

	//Checking whether or not there is a value inserted in our table
	bool empty() const {

		if (this->buckets.size() == 0) {
			return true;
		}
		return false;
	}

	//Function to return 0 or 1 for the particular key provided
	size_type count(const key_type& key) {
		return search(key);
	}

	//Return iterator pointed to the bucket if key is found,
	//else iterator is pointed to null
	iterator find(const key_type& key) const {
		
		return const_iterator{ this,searchh(key),bucketsize };
		
	}

	//Swapping elements
	void swap(ADS_set& other) {

		std::swap(bucketsize, other.bucketsize);
		std::swap(buckets , other.buckets);
	}

	//Returns pair of iterator and bool whether it 
	//exist in the hashtable or not
	std::pair<iterator, bool> iinsert(const_reference key) {
		if (!count(key)) {
			insert(key);
			return std::make_pair(find(key), true);
		}
		return std::make_pair(find(key), false);
	}


	//Returns start buckets refrence of the hashtable,
	//either it is present at 0 or 1 or 2
	const_iterator begin() const {
		for (size_t i = 0; i < buckets.size(); i++)
			if (!buckets[i]->values.empty()) {
				return const_iterator(this, buckets[i]->values, i, buckets.size());
			}
		return end();
	}


	const_iterator end() const { return const_iterator(nullptr); }


	////////////////_REQUIRED OVERLOADS_//////////////////////////////////////

	//Compare 2 ADS_set objects and return true or false if they are equal
	friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) {
		if (lhs.size() != rhs.size())
			return false;

		for (const auto& key : lhs)
			if (!rhs.count(key))
				return false;

		return true;
	}

	friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs) {
		return !(lhs == rhs);
	}

	//Finish

};

template <typename Key, size_t N>
class ADS_set<Key, N>::Iterator {
private:
	const ADS_set<Key, N>* ptr;
	Bucket<Key>* to;
	size_type itPos;
	size_type tblSz;

public:
	using value_type = Key;
	using difference_type = std::ptrdiff_t;
	using reference = const value_type&;
	using pointer = const value_type*;
	using iterator_category = std::forward_iterator_tag;

	explicit Iterator(const ADS_set* ads = nullptr, Bucket<Key>* tbl = nullptr,size_type idx = 0, size_type sz = 0): ptr(ads), to(tbl), itPos(idx), tblSz(sz) 
	{
		//std::cout << "ITERATOR_CONSTRUCTOR" << '\n';
	}

	reference operator*() const {
		return to->data;
	}

	pointer operator->() const {
		return &to->data;
	}

	Iterator& operator++() {
		while (itPos < tblSz) {
			if (to->next) { to = to->next; return *this; }
			else ++itPos;

			if (itPos == tblSz) { to = nullptr; return *this; } //!

			auto transit = ptr->table[itPos].head;
			if (transit) { to = transit; return *this; }
		}
		return *this;
	}

	Iterator operator++(int) {
		Iterator tmp(*this);
		operator++();

		return tmp;
	}

	friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
		return lhs.to == rhs.to;
	}
	friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
		return !(lhs.to == rhs.to);
	}
};



int main() {
	int bucket_size;
	cout << " Input Bucket Size: ";
	cin >> bucket_size;
	ADS_set <int,2>h(bucket_size);
	int inp;

	cout << " \n 1) Insert Key\n 2) Seach key\n 3) Delete key\n 4) Display\n 5) Exit\n Enter Input: ";
	cin >> inp;
	while (inp != 5) {
		if (inp == 1) {
			int key;
			cout << "\n Enter Key: ";
			cin >> key;
			h.insert(key);
		}
		else if (inp == 2) {
			int key;
			cout << "\n Enter Key: ";
			cin >> key;
			
			if (h.search(key)) {
				cout << "Key Found";
			}
			else {
				cout << " Key Not Found";
			}
		}
		else if (inp == 3) {
			int key;
			cout << "\n Enter Key: ";
			cin >> key;
			h.del(key);
		}
		else if (inp == 4) {
			h.display();
		}
		else if (inp == 5) {
			break;
		}
		cout << "\n Enter Input: ";
		cin >> inp;
	}
	return 1;

}