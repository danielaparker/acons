#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cstdlib> //std::system
#include <acons/ndarray.hpp>
#include <iostream>

using namespace acons;

typedef boost::interprocess::allocator<double,
        boost::interprocess::managed_shared_memory::segment_manager> shmem_allocator;

typedef ndarray<double,2,row_major,zero_based,shmem_allocator> ndarray_shm;

int main(int argc, char *argv[])
{
    typedef std::pair<double, int> MyType;

    if(argc == 1) //Parent process
    {  
       //Remove shared memory on construction and destruction
       struct shm_remove
       {
          shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
          ~shm_remove(){ boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
       } 
       remover;

       //Construct managed shared memory
       boost::interprocess::managed_shared_memory segment(boost::interprocess::create_only, 
                                                          "MySharedMemory", 65536);

       //Initialize shared memory STL-compatible allocator
       const shmem_allocator allocator(segment.get_segment_manager());

       // Create ndarray with all dynamic allocations in shared memory
       ndarray_shm* pA = segment.construct<ndarray_shm>("my ndarray")(std::array<size_t,2>{2,2}, 0.0, allocator);
       ndarray_shm& a = *pA;

       a(0,0) = 0;
       a(0,1) = 1;
       a(1,0) = 2;
       a(1,1) = 3;

       std::pair<ndarray_shm*, boost::interprocess::managed_shared_memory::size_type> res;
       res = segment.find<ndarray_shm>("my ndarray");

       std::cout << "Parent process:\n";
       std::cout << *(res.first) << "\n";

       //Launch child process
       std::string s(argv[0]); s += " child ";
       if(0 != std::system(s.c_str()))
          return 1;

       //Check child has destroyed all objects
       if(segment.find<MyType>("my ndarray").first)
          return 1;
    }
    else
    {
       //Open managed shared memory
       boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, 
                                                          "MySharedMemory");

       std::pair<ndarray_shm*, boost::interprocess::managed_shared_memory::size_type> res;
       res = segment.find<ndarray_shm>("my ndarray");

       if (res.first != nullptr)
       {
           std::cout << "\nChild process:\n";
           std::cout << *(res.first) << "\n";
       }
       else
       {
           std::cout << "Result is null\n";
       }

       //We're done, delete all the objects
       segment.destroy<ndarray_shm>("my ndarray");
    }
}

