set(LLVM_LINK_COMPONENTS
  BitWriter
  Core
  Support
  )

add_llvm_tool(llvm-example
  llvm-example.cpp
  )
