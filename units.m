classdef units < handle & matlab.mixin.CustomDisplay
    properties
       val = 0;
       unit = {""};
       pow = {1};
       compound = false;
    end
    methods
        function this = units(x, u, p)
            this.compound = iscell(u);
            this.val = x; this.unit = u; this.pow = p;
            this = units_C(this);
        end
        function ret = plus(u1, u2)
            ret = units_C('Plus', u1, u2);
        end
        function ret = uminus(u1)
            ret = units(-u1.val, u1.unit, u1.pow);            
        end
        function ret = minus(u1, u2)
            ret = units_C('Plus', u1, -u2);
        end
    end
    
    methods (Access = protected)
        function displayScalarObject(obj)
            if(obj.pow==1)
                fprintf('%.3f %s\n',obj.val,obj.unit);
            else
                fprintf('%.3f %s^%f\n',obj.val,obj.unit,obj.pow);
            end
        end
    end
end