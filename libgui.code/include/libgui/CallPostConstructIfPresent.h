//
// Created by Bob Kocisko on 4/27/17.
//

#pragma once

#include <memory>

namespace libgui
{

/**
 * has_postconstruct
 *
 * has_postconstruct::value will be true when the specified type T
 * has the method PostConstruct()
 *
 * Thanks to: http://stackoverflow.com/a/257382/4307047
 */
template<typename T>
class has_postconstruct
{
  typedef char one;
  typedef long two;

  #pragma clang diagnostic push
  #pragma ide diagnostic ignored "NotImplementedFunctions"
  template<typename C>
  static one test(decltype(&C::PostConstruct));
  template<typename C>
  static two test(...);
  #pragma clang diagnostic pop

public:
  enum
  {
    value = sizeof(test<T>(0)) == sizeof(char)
  };
};

/**
 * Call PostConstruct() on the specified element
 */
template<class T>
auto CallPostConstructIfPresent(std::shared_ptr<T> element) ->
typename std::enable_if<has_postconstruct<T>::value, void>::type
{
  element->PostConstruct();
}

/**
 * Don't call PostConstruct() on the specified element because the PostConstruct() method doesn't exist in that type
 */
template<class T>
auto CallPostConstructIfPresent(std::shared_ptr<T> element) ->
typename std::enable_if<!has_postconstruct<T>::value, void>::type
{
}


}
