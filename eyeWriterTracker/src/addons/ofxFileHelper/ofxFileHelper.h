/***********************************************************************
 Created by Theodore Watson. 
 Copywrite Theodore Watson 2009, 2010.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***********************************************************************/

/*
  BIG NOTICE! 
  CODE THAT MOVES/RENAMES/DELETES FILES IS RISKY BUSINESS. 
  ALWAYS HAVE A BACKUP OF YOUR DATA AND UNDERSTAND THAT BUGS IN THE CODE AS WELL AS PASSING A PATH IN THE WRONG FORMAT MAY RESULT IN YOUR FILES BEING DELETED!
  HAVE A NICE DAY :) 
*/

#pragma once

#include <filesystem>
#include <string>

#include "ofFileUtils.h"
#include "ofLog.h"

namespace fs = std::filesystem;
using std::string;

class ofxFileHelper{
  public:
  static bool copyFromTo(string pathSrc, string pathDst, bool bRelativeToData = true, bool overwrite = false){
	  if(bRelativeToData){
		  pathSrc = ofToDataPath(pathSrc);
		  pathDst = ofToDataPath(pathDst);
	  }

	  fs::path src(pathSrc);
	  fs::path dst(pathDst);

	  if(!fs::exists(src)){
		  return false;
	  }

	  if(fs::exists(dst)){
		  if(overwrite){
			  if(!deleteFolder(pathDst, false)){
				  return false;
			  }
		  }else{
			  return false;
		  }
	  }

	  std::error_code ec;

	  if(fs::is_directory(src)){
		  fs::create_directories(dst, ec);
		  if(ec){
			  ofLogError("ofxFileHelper") << "copyFromTo - unable to ensure destination directory '" << dst.string()
										  << "': " << ec.message();
			  return false;
		  }
		  ec.clear();
		  fs::copy(src, dst, fs::copy_options::recursive | fs::copy_options::copy_symlinks, ec);
		  if(ec){
			  ofLogError("ofxFileHelper") << "copyFromTo - unable to copy directory '" << src.string()
										  << "' to '" << dst.string() << "': " << ec.message();
			  return false;
		  }
	  }else{
		  auto parent = dst.parent_path();
		  if(!parent.empty()){
			  fs::create_directories(parent, ec);
			  if(ec){
				  ofLogError("ofxFileHelper") << "copyFromTo - unable to create parent directory '" << parent.string()
											  << "': " << ec.message();
				  return false;
			  }
			  ec.clear();
		  }
		  fs::copy_file(src, dst, fs::copy_options::none, ec);
		  if(ec){
			  ofLogError("ofxFileHelper") << "copyFromTo - unable to copy file '" << src.string()
										  << "' to '" << dst.string() << "': " << ec.message();
			  return false;
		  }
	  }

	  return true;
  }

  static bool moveFromTo(string pathSrc, string pathDst, bool bRelativeToData = true, bool overwrite = false){
	  if(bRelativeToData){
		  pathSrc = ofToDataPath(pathSrc);
		  pathDst = ofToDataPath(pathDst);
	  }

	  fs::path src(pathSrc);
	  fs::path dst(pathDst);

	  if(!fs::exists(src)){
		  return false;
	  }

	  if(fs::exists(dst)){
		  if(overwrite){
			  if(!deleteFolder(pathDst, false)){
				  return false;
			  }
		  }else{
			  return false;
		  }
	  }

	  std::error_code ec;
	  auto parent = dst.parent_path();
	  if(!parent.empty()){
		  fs::create_directories(parent, ec);
		  if(ec){
			  ofLogError("ofxFileHelper") << "moveFromTo - unable to create destination directory '" << parent.string()
										  << "': " << ec.message();
			  return false;
		  }
		  ec.clear();
	  }

	  fs::rename(src, dst, ec);
	  if(!ec){
		  return true;
	  }

	  ofLogWarning("ofxFileHelper") << "moveFromTo - rename failed (" << ec.message()
									<< "), attempting copy/remove fallback";

	  if(!copyFromTo(pathSrc, pathDst, false, overwrite)){
		  return false;
	  }

	  if(!deleteFolder(pathSrc, false)){
		  ofLogWarning("ofxFileHelper") << "moveFromTo - copied to destination but failed to remove original '"
										<< src.string() << "'";
		  return false;
	  }

	  return true;
  }

  static string getFileExt(const string& filename){
	  std::string::size_type idx = filename.rfind('.');
	  return (idx != std::string::npos) ? filename.substr(idx + 1) : "";
  }

  static string removeExt(const string& filename){
	  std::string::size_type idx = filename.rfind('.');
	  return (idx != std::string::npos) ? filename.substr(0, idx) : filename;
  }

  static string prependBackslash(string path){
	  if(!path.empty() && path.front() != '/'){
		  path.insert(path.begin(), '/');
	  }
	  return path;
  }

  static string appendBackslash(string path){
	  if(!path.empty() && path.back() != '/'){
		  path.push_back('/');
	  }
	  return path;
  }

  static bool doesDirectoryExist(string dirPath, bool bRelativeToData = true){
	  if(bRelativeToData) dirPath = ofToDataPath(dirPath);
	  std::error_code ec;
	  return fs::exists(dirPath, ec) && !ec;
  }

  static bool doesFileExist(string fPath, bool bRelativeToData = true){
	  if(bRelativeToData) fPath = ofToDataPath(fPath);
	  std::error_code ec;
	  return fs::exists(fPath, ec) && !ec;
  }

  static bool makeDirectory(string dirPath, bool bRelativeToData = true){
	  if(bRelativeToData) dirPath = ofToDataPath(dirPath);
	  std::error_code ec;
	  bool created = fs::create_directories(dirPath, ec);
	  if(ec){
		  ofLogError("ofxFileHelper") << "makeDirectory - directory could not be created: " << ec.message();
		  return false;
	  }
	  if(!created){
		  ofLogWarning("ofxFileHelper") << "makeDirectory - directory already exists";
	  }
	  return created;
  }

  static bool deleteFile(string path, bool bRelativeToData = true){
	  if(bRelativeToData) path = ofToDataPath(path);
	  std::error_code ec;
	  bool removed = fs::remove(path, ec);
	  if(ec || !removed){
		  ofLogError("ofxFileHelper") << "deleteFile - file could not be deleted"
									  << (ec ? (": " + ec.message()) : "");
		  return false;
	  }
	  return true;
  }

  static bool deleteFolder(string path, bool bRelativeToData = true){
	  if(bRelativeToData) path = ofToDataPath(path);
	  std::error_code ec;
	  auto removed = fs::remove_all(path, ec);
	  if(ec || removed == 0){
		  ofLogError("ofxFileHelper") << "deleteFolder - path could not be deleted"
									  << (ec ? (": " + ec.message()) : "");
		  return false;
	  }
	  return true;
  }
};

//
//#pragma once
//
//#include "ofMain.h"
//
//#include "Poco/Util/FilesystemConfiguration.h"
//#include "Poco/File.h"
//#include "Poco/Path.h"
//#include "Poco/DirectoryIterator.h"
//#include "Poco/StringTokenizer.h"
//#include "Poco/Exception.h"
//#include "Poco/FileStream.h"
//
//
//using Poco::Path;
//using Poco::File;
//using Poco::DirectoryIterator;
//using Poco::StringTokenizer;
//using Poco::NotFoundException;
//
//class ofxFileHelper{
//	public:
//
//	//------------------------------------------------------------------------------------------------------------
//	static bool copyFromTo(string pathSrc, string pathDst, bool bRelativeToData = true,  bool overwrite = false){
//		if( bRelativeToData ) pathSrc = ofToDataPath(pathSrc);
//		if( bRelativeToData ) pathDst = ofToDataPath(pathDst);
//
//		if( !ofxFileHelper::doesFileExist(pathSrc) ){
//            return false;
//		}
//
//        if( ofxFileHelper::doesFileExist(pathDst) ){
//            if( overwrite ){
//                ofxFileHelper::deleteFolder(pathDst);
//            }else{
//                return false;
//            }
//        }
//
//		File fileSrc(pathSrc);
//		try{
//            fileSrc.copyTo(pathDst);
//		}catch (Poco::Exception & except){
//		    ofLog(OF_LOG_ERROR, "copyFromTo - unable to copy to, file must be busy");
//		    return false;
//		}
//		return true;
//	}
//
//	//be careful with slashes here - appending a slash when moving a folder will causes mad headaches
//	//------------------------------------------------------------------------------------------------------------
//	static bool moveFromTo(string pathSrc, string pathDst, bool bRelativeToData = true, bool overwrite = false){
//		if( bRelativeToData ) pathSrc = ofToDataPath(pathSrc);
//		if( bRelativeToData ) pathDst = ofToDataPath(pathDst);
//
//        if( !ofxFileHelper::doesFileExist(pathSrc) ){
//            return false;
//		}
//
//        if( ofxFileHelper::doesFileExist(pathDst) ){
//            if( overwrite ){
//                ofxFileHelper::deleteFolder(pathDst);
//            }else{
//                return false;
//            }
//        }
//
//		File fileSrc(pathSrc);
//
//        try{
//            fileSrc.moveTo(pathDst);
//		}catch (Poco::Exception & except){
//		    ofLog(OF_LOG_ERROR, "moveFromTo - unable to move to, file must be busy");
//		    return false;
//		}
//        return true;
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static string getFileExt(string filename){
//		std::string::size_type idx;
//		idx = filename.rfind('.');
//
//		if(idx != std::string::npos){
//			return filename.substr(idx+1);
//		}
//		else{
//			return "";
//		}
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static string removeExt(string filename){
//		std::string::size_type idx;
//		idx = filename.rfind('.');
//
//		if(idx != std::string::npos){
//			return filename.substr(0, idx);
//		}
//		else{
//			return filename;
//		}
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static string prependBackslash(string path){
//		if(path.length() > 0){
//			if( path[0] != '/'){
//				path = "/"+path;
//			}
//		}
//		return path;
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static string appendBackslash(string path){
//		if(path.length() > 0){
//			if( path[path.length()-1] != '/'){
//				path += "/";
//			}
//		}
//		return path;
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static bool doesDirectoryExist(string dirPath, bool bRelativeToData = true){
//		if( bRelativeToData ) dirPath = ofToDataPath(dirPath);
//		File myFile( dirPath );
//		return myFile.exists();
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static bool doesFileExist(string fPath,  bool bRelativeToData = true){
//		if( bRelativeToData ) fPath = ofToDataPath(fPath);
//		File myFile( fPath );
//		return myFile.exists();
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static bool makeDirectory(string dirPath, bool bRelativeToData = true){
//		if( bRelativeToData ) dirPath = ofToDataPath(dirPath);
//
//		File myFile(dirPath);
//		bool success = false;
//		try{
//            success = myFile.createDirectory();
//		}catch( Poco::Exception &except ){
//            ofLog(OF_LOG_ERROR, "makeDirectory - directory could not be created");
//            return false;
//		}
//
//        if(!success)ofLog(OF_LOG_WARNING, "makeDirectory - directory already exists");
//
//		return success;
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static bool deleteFile(string path, bool bRelativeToData = true){
//	    if( bRelativeToData ) path = ofToDataPath(path);
//		File myFile(path);
//		try{
//            myFile.remove();
//		}catch( Poco::Exception &except){
//            ofLog(OF_LOG_ERROR, "deleteFile - file could not be deleted");
//            return false;
//		}
//		return true;
//	}
//
//	//------------------------------------------------------------------------------------------------------------
//	static bool deleteFolder(string path, bool bRelativeToData = true){
//	    if( bRelativeToData ) path = ofToDataPath(path);
//		File myFile(path);
//		try{
//            myFile.remove(true);
//		}catch(Poco::Exception &except){
//            ofLog(OF_LOG_ERROR, "deleteFile - folder could not be deleted");
//            return false;
//		}
//		return true;
//	}
//
//};
