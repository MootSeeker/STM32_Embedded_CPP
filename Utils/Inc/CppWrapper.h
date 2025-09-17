/**
 * Filenaem: CppWrapper.h
 *
 */

#ifndef INC_CPP_WRAPPER_H_
#define INC_CPP_WRAPPER_H_

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * @brief C Event loop
	 * @details wrapper for CPP since CubeMX does not allow CPP code generation
	 */
	void EventLoopC( void );

#ifdef __cplusplus
}
#endif


#endif /* INC_CPP_WRAPPER_H_ */
