#pragma once

//
// C/C++
//
#include <cstdlib>
#include <crtdbg.h>
#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <atomic>
#include <memory>
#include <utility>
#include <cassert>
#include <functional>
#include <sstream>
#include <csignal>
#include <limits>
#include <fstream>
#include <iostream>

//
// boost
//

namespace std
{

	//
	// In C++11 std::unary_function template are deprecated
	// In C++17 it was removed
	// We use /std:c++latest flag for compile this project
	// And therefore boost::lexical_cast does not compiled
	// Because it uses removed std::unary_function template
	//
	// DELETE THIS AFTER CHANGING BOOST ON LATER VERSION WHERE IT WILL BE FIXED
	//
	// http://en.cppreference.com/w/cpp/utility/functional/unary_function
	// https://svn.boost.org/trac10/ticket/12972
	//

	template <class _Arg, class _Result>
	struct unary_function
	{
		typedef _Arg argument_type;
		typedef _Result result_type;
	};

}

#include <boost/functional/hash.hpp>
#include <boost/any.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>

using namespace std::chrono_literals;