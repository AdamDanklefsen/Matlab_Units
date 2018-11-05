#include "mex.hpp"
#include "mexAdapter.hpp"
#include <string>
#include <map>

using matlab::mex::ArgumentList;
using namespace matlab::data;
typedef std::shared_ptr<matlab::engine::MATLABEngine> mPtr;
std::ostringstream stream;

double pi = 3.141592658579;
typedef std::map<std::string, int> strImap;
strImap unitType = {
	{"m", 0},
	{"mm", 1},
	{"ft", 2},
	{"in", 3},
	{"kg", 4},
	{"slug", 5},
	{"s", 6},
	{"h", 7},
	{"min", 8},
	{"N", 9},
	{"kN", 10},
	{"lb", 11},
	{"kip", 12},
	{"Pa", 13},
	{"kPa", 14},
	{"MPa", 15},
	{"GPa", 16},
	{"psi", 17},
	{"ksi", 18},
	{"psf", 19},
	{"ksf", 20},
	{"J", 21},
	{"kJ", 22},
	{"MJ", 23},
	{"BTU", 24},
	{"W", 25},
	{"kW", 26},
	{"MW", 27},
	{"hp", 28},
	{"rpm", 29},
	{"rad_s", 30},
	{"m_s", 31},
	{"ft_s", 32},
	{"_", 33}
};
typedef std::map<std::string, double> strDmap;
strDmap conv = {
	{"m", 1},
	{"mm", .001},
	{"ft", 1},
	{"in", 1 / 12.f},
	{"kg", 1},
	{"slug", 1},
	{"s", 1},
	{"h", 3600},
	{"min", 60},
	{"N", 1},
	{"kN", 1000},
	{"lb", 1},
	{"kip", 1000},
	{"Pa", .001},
	{"kPa", 1},
	{"MPa", 1000},
	{"GPa", 1000000},
	{"psi", 1},
	{"ksi", 1000},
	{"psf", 1 / 144.f},
	{"ksf", 1 / 144.f*1000.f},
	{"J", .001},
	{"kJ", 1},
	{"MJ", 1000},
	{"BTU", 1},
	{"W", .001},
	{"kW", 1},
	{"MW", 1000},
	{"hp", 1},
	{"rpm", 2.f*pi / 60.f},
	{"rad_s", 1},
	{"m_s", 1},
	{"ft_s", 1}
};
typedef std::map<std::string, std::string> strstrmap;
strstrmap convUnit = {
	{"m", "m"},
	{"mm", "m"},
	{"ft", "ft"},
	{"in", "ft"},
	{"kg", "kg"},
	{"slug", "slug"},
	{"s", "s"},
	{"h", "s"},
	{"min", "s"},
	{"N", "N"},
	{"kN", "N"},
	{"lb", "lb"},
	{"kip", "lb"},
	{"Pa", "kPa"},
	{"kPa", "kPa"},
	{"MPa", "kPa"},
	{"GPa", "kPa"},
	{"psi", "psi"},
	{"ksi", "psi"},
	{"psf", "psi"},
	{"ksf", "psi"},
	{"J", "kJ"},
	{"kJ", "kJ"},
	{"MJ", "kJ"},
	{"BTU", "BTU"},
	{"W", "kW"},
	{"kW", "kW"},
	{"MW", "kW"},
	{"hp", "hp"},
	{"rpm", "rad_s"},
	{"rad_s", "rad_s"},
	{"m_s", "m_s"},
	{"ft_s", "ft_s"}
};


std::string Mstr_str(TypedArray<char16_t>& arr){
		std::string s;
		for(auto it = arr.begin(); it != arr.end(); it++){
			char16_t a = std::move(*it);
			s += a;
		}
		return s;
	}
	bool isMstr_str(std::string s, TypedArray<char16_t>& arr){
		return(!s.compare(Mstr_str(arr)));
	}

class units {
public:
	units(double x, std::string s, int p) : val(x), unit(s), pow(p) {}
	units(mPtr matlabPtr, matlab::data::Array obj) {
		matlab::data::Array val = matlabPtr->getProperty(obj, u"val");
        TypedArray<char16_t> str = matlabPtr->getProperty(obj, u"unit");
		matlab::data::Array pow = matlabPtr->getProperty(obj, u"pow");
		double x = std::move(val[0]);
		std::string s = Mstr_str(str);
		int p = std::move(pow[0]);
		units d(x,s,p);
		*this = d;
	}
	~units() {}
	void norm() {
		val *= conv.find(unit)->second;
		unit = convUnit.find(unit)->second;
	}
	bool isUnit() {
		return(unitType.find(unit) != unitType.end());
	}

	double val = 0;
	std::string unit = "";
	int pow = 1;
};

class MexFunction : public matlab::mex::Function {
	mPtr matlabPtr = getEngine();
	ArrayFactory factory;
public:
	void operator()(ArgumentList outputs, ArgumentList inputs) {
		if(inputs[0].getType() == ArrayType::CHAR && (inputs.size() == 2 || inputs.size() == 3)){ // operators
			TypedArray<char16_t> str = std::move(inputs[0]);
			if(isMstr_str("Plus", str)) {
				if(inputs[1].getType() == ArrayType::HANDLE_OBJECT_REF && inputs[2].getType() == ArrayType::HANDLE_OBJECT_REF){
					auto u1 = units(matlabPtr, inputs[1]); auto u2 = units(matlabPtr, inputs[2]);
					u1.norm(); u2.norm();
					if(u1.unit.compare(u2.unit)){
						std::string s = "DankErr: Mismatched Units { " + u1.unit + " + " + u2.unit + " }";
						throw std::runtime_error(s);
					}
					auto sum = units(u1.val+u2.val, u1.unit, u1.pow);
					outputs[0] = inputs[1];
					CreateMatUnits(outputs, sum);
				}else{
					throw std::runtime_error("DankErr: Plus Takes Two Unit Objects");
				}
			}else if(isMstr_str("Times", str)){
				if(inputs[1].getType() == ArrayType::HANDLE_OBJECT_REF && inputs[2].getType() == ArrayType::HANDLE_OBJECT_REF){
					auto u1 = units(matlabPtr, inputs[1]); auto u2 = units(matlabPtr, inputs[2]);
					u1.norm(); u2.norm();
				}else{
					throw std::runtime_error("DankErr: Times Takes Two Unit Objects");
				}

			}else if(isMstr_str("Pow", str)){
				if(inputs[1].getType() == ArrayType::HANDLE_OBJECT_REF){

				}else{
					throw std::runtime_error("DankErr: Pow Takes One Unit Object");
				}
			}else{
				throw std::runtime_error("DankErr: Invalid Operator Syntax");
			}
		} else if(inputs[0].getType() == ArrayType::HANDLE_OBJECT_REF){ // Constructor
			// Move object to variable
			Array obj = std::move(inputs[0]);

			// Get prop from MATLAB and store
			matlab::data::Array val = matlabPtr->getProperty(obj, u"val");
        	TypedArray<char16_t> str = matlabPtr->getProperty(obj, u"unit");
			matlab::data::Array pow = matlabPtr->getProperty(obj, u"pow");
			double x = std::move(val[0]);
			std::string s = Mstr_str(str);
			int p = std::move(pow[0]);
			units u(x,s,p);
			if (!u.isUnit()) { throw std::runtime_error({ "DankErr: Invalid Unit " + u.unit }); }

			// Set Prop
			matlabPtr->setProperty(obj, u"val", factory.createScalar(u.val));
			matlabPtr->setProperty(obj, u"unit", factory.createCharArray(u.unit));
			matlabPtr->setProperty(obj, u"pow", factory.createScalar(u.pow));
			outputs[0] = obj;
		}else{
			throw std::runtime_error("DankErr: Invalid Syntax");
		}
	}

	void CreateMatUnits(ArgumentList & argL, units &u){
		Array obj = std::move(argL[0]);
		matlabPtr->setProperty(obj, u"val", factory.createScalar(u.val));
		matlabPtr->setProperty(obj, u"unit", factory.createCharArray(u.unit));
		matlabPtr->setProperty(obj, u"pow", factory.createScalar(u.pow));
		argL[0] = obj;
	}

	void displayOnMATLAB(std::ostringstream& stream) {
		// Pass stream content to MATLAB fprintf function
		matlabPtr->feval(u"fprintf", 0,
			std::vector<Array>({ factory.createScalar(stream.str()) }));
		// Clear stream buffer
		stream.str("");
	}
};