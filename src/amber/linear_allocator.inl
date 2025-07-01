namespace amber {

template<typename T>
T* linear_allocator::allocate(std::size_t count)
{
    return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
}

template<typename T>
T* linear_allocator::allocate()
{
    return allocate<T>(1);
}

template<typename T>
T* linear_allocator::try_allocate(std::size_t count) noexcept
{
    return static_cast<T*>(try_allocate(sizeof(T) * count, alignof(T)));
}

template<typename T>
T* linear_allocator::try_allocate() noexcept
{
    return try_allocate<T>(1);
}

}
