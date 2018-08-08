#pragma once

//Fails in release mode
//using TypeID = void(*)();

//template<class T>
//TypeID TypeId() noexcept
//{
//	return TypeID(TypeId<T>);
//}