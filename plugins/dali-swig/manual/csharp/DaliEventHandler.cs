namespace Dali {

	using System;
	using System.Runtime.InteropServices;

	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	public delegate void DaliEventHandler<T,U>(T source, U e);

	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	public delegate R DaliEventHandlerWithReturnType<T,U,R>(T source, U e);

}
