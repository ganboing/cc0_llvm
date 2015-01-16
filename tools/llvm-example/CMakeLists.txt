set(LLVM_LINK_COMPONENTS
  BitReader
  Core
  Support
  )

add_llvm_tool(llvm-example
  llvm-example.cpp
  )
