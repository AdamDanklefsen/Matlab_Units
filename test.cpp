#include "mex.hpp"
#include "mexAdapter.hpp"

class unit {
public:
	unit() {}
	~unit() {}
	int val = 0;
};

using matlab::mex::ArgumentList;
using namespace matlab::data;

class MexFunction : public matlab::mex::Function {
	std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
public:
	void operator()(ArgumentList outputs, ArgumentList inputs) {

		// Move object to variable
		Array obj = std::move(inputs[0]);

		matlab::data::Array val = matlabPtr->getProperty(obj, u"val");
		matlabPtr->setProperty(obj, u"val", val);
		matlab::data::String str = matlabPtr->getProperty(obj, u"unit");
		matlabPtr->setProperty(obj, u"unit", str);
		matlab::data::Array pow = matlabPtr->getProperty(obj, u"pow");
		matlabPtr->setProperty(obj, u"pow", pow);

		mexErrMsgText("Bitch");
		outputs[0] = obj;
	}
};