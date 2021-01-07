#pragma once

#include <unordered_map>
#include <boost/preprocessor.hpp>

#define GEN_TOKEN(elem) \
	BOOST_PP_CAT({, \
	BOOST_PP_CAT(BOOST_PP_STRINGIZE(elem), \
	BOOST_PP_CAT(BOOST_PP_COMMA(), \
	BOOST_PP_CAT(reinterpret_cast<void*>), \
	)))(
#define GEN_TOKEN2(elem) \
	GEN_TOKEN(elem)&(
#define GEN_TOKEN3(r,data,elem) \
	GEN_TOKEN2(elem)elem))},

#define CHX_META_GROUP(...) \
	{BOOST_PP_SEQ_FOR_EACH(GEN_TOKEN3,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)){"__end__",nullptr}}

#define CHX_META(_Name, ...) \
	MetaBase(#_Name,CHX_META_GROUP(__VA_ARGS__))

namespace chromo {
	class MetaBase {
	protected:
		std::string _Name;
		std::unordered_map<std::string, void*> _Property;

	public:
		MetaBase(const std::string& name,
			const std::unordered_map<std::string, void*>& prop) :
			_Name(name), _Property(prop) {

		}
		MetaBase(const std::string& name) :
			_Name(name) {

		}
		const auto& get_object_name() {
			return _Name;
		}
		template<typename T>
		const T* const_property(const std::string& name) {
			auto ite = _Property.find(name);
			if (ite != _Property.end()) {
				return static_cast<const T*>(ite->second);
			}
			else {
				return nullptr;
			}
		}
		template<typename T>
		T* property(const std::string& name) {
			auto ite = _Property.find(name);
			if (ite != _Property.end()) {
				return static_cast<T*>(ite->second);
			}
			else {
				return nullptr;
			}
		}
	};
}