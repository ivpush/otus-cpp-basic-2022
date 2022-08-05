// OTUS C++ basic course
// 

#include <limits>
#include <numeric>
#include <vector>


// Abstract Interface Class for Statistics
//
class IStatistics {
public:
	virtual ~IStatistics() {}

	virtual void update(double next) = 0;   // update method value on iteration
	virtual double eval() const = 0;        // return method value
	virtual const char * name() const = 0;  // return method name
};

// min - mimimal value of sequence 
// 
class Min : public IStatistics {
public:
	Min() : m_min{std::numeric_limits<double>::max()} {
	}

	void update(double next) override {
		if (next < m_min) {
			m_min = next;
		}
	}

	double eval() const override {
		return m_min;
	}

	const char * name() const override {
		return "min value";
	}

private:
	double m_min;   // minimum itself
};

// max - maximal value of sequence 
// 
class Max : public IStatistics {
public:
	Max() : m_max{std::numeric_limits<double>::min()} {
	}

	void update(double next) override {
		if (next > m_max) {
			m_max = next;
		}
	}

	double eval() const override {
		return m_max;
	}

	const char * name() const override {
		return "max value";
	}

private:
	double m_max;   // maximum itself
};

// mean - math average value of all sequence 
// 
class Mean : public IStatistics {
public:
	Mean() : m_sum (0), m_cnt (0)  {
	}

	void update(double next) override {
		m_sum += next;
		++m_cnt;
	}

	double eval() const override {
		return m_sum/m_cnt;
	}

	const char * name() const override {
		return "average value";
	}

private:
	double  m_sum;   // maximum itself
    int     m_cnt; 
};

// std - average quad deviation value of all sequence 
// 
class Std : public IStatistics {
public:
	Std()  {
	}

	void update(double next) override {
		m_vector.push_back (next);
	}

	double eval() const override {
		double mean = std::accumulate (std::begin(m_vector), std::end(m_vector), 0.0) / m_vector.size();

		double accum = 0.0;
		for ( auto d : m_vector )
    		accum += (d - mean) * (d - mean);

		return sqrt(accum / m_vector.size());
	}

	const char * name() const override {
		return "standard deviation";
	}

private:
	std::vector <double>  m_vector;   // numbers collection
};

