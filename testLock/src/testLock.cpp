#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

using namespace std;

class LockFile
{
  public:

    LockFile(string name)
    {
      lockfile = name;
    }

    int tryGetLock();
    void releaseLock(int fd);
    void boostLock(string name);
    void boostSharedLock(string name);
    

  private:
    string lockfile;
};

void LockFile::boostLock(string name)
{
  cout << "boostLock(" << name << ")" << endl;
  boost::interprocess::file_lock fl(name.c_str());
  {
    cout << "boostLock: acquiring e_lock..." << endl;
    boost::interprocess::scoped_lock<boost::interprocess::file_lock> e_lock(fl);
    cout << "boostLock: lock file [" << name << "] acquired, sleep 10s..." << endl;
    sleep(10);
  }

  cout << "done sleep, relase e_lock." << endl;
}

void LockFile::boostSharedLock(string name)
{
  cout << "boostLock(" << name << ")" << endl;
  boost::interprocess::file_lock fl(name.c_str());
  {
    cout << "boostLock: acquiring s_lock..." << endl;
    boost::interprocess::sharable_lock<boost::interprocess::file_lock> s_lock(fl);
    cout << "boostLock: lock file [" << name << "] acquired, sleep 10s..." << endl;
    sleep(10);
  }

  cout << "done sleep, relase s_lock." << endl;

}


int LockFile::tryGetLock()
{
    mode_t m = umask( 0 );
    int fd = open(lockfile.c_str(), O_RDWR|O_CREAT, 0666 );
    umask( m );
    if( fd >= 0 && flock( fd, LOCK_EX | LOCK_NB ) < 0 )
    {
        close( fd );
        fd = -1;
    }
    return fd;
}

void LockFile::releaseLock(int fd)
{
    if( fd < 0 )
        return;
    close( fd );
//    unlink(lockfile.c_str()); 
}

int main(void)
{
  string fname= "/home/jackey/tmp.lock";
  LockFile lf(fname);

  cout << "try lock file [" << fname << "]..." << endl;
  int fd = lf.tryGetLock();

  if (fd != -1)
  {
    cout << "lock success" << endl;
  }
  else
  {
    cout << "lock failed" << endl;
  }

//  sleep(10);

  string h = "hello";

  if (fd != -1)
  {
    lseek(fd, 0, SEEK_SET);
    write(fd, h.c_str(), sizeof(h.c_str()));
    lf.releaseLock(fd);
  }

//  lf.boostLock("/mnt/rmsdata/test.txt");
  lf.boostSharedLock("/mnt/rmsdata/test.txt");
}
