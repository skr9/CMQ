if(EXISTS "/home/CMQ/test/message/build/msg_test[1]_tests.cmake")
  include("/home/CMQ/test/message/build/msg_test[1]_tests.cmake")
else()
  add_test(msg_test_NOT_BUILT msg_test_NOT_BUILT)
endif()
