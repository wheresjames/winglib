/**
 * The "bind()" function extension from Prototype.js, extracted for general use
 *
 * @author Richard Harrison, http://www.pluggable.co.uk
 * @author Sam Stephenson (Modified from Prototype Javascript framework)
 * @license MIT-style license @see http://www.prototypejs.org/
 */
Function.prototype.bind = function()
{   var _$A = function(a){return Array.prototype.slice.call(a);}
    if(arguments.length < 2 && (typeof arguments[0] == "undefined")) return this;
    var __method = this, args = _$A(arguments), object = args.shift();
    return function() { return __method.apply(object, args.concat(_$A(arguments))); }
}
