#pragma once

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using WeakPtr = std::shared_ptr<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;