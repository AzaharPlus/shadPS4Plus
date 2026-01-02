// adapted from InstallDragDropPkg in main_window.cpp

#include <iostream>
#include "core/file_format/pkg.h"
#include "core/file_format/psf.h"
#include "common/path_util.h"
#include "common/string_util.h"
#include "core/loader.h"

int main(int argc, char** argv){
	std::filesystem::path file = "";
	std::filesystem::path output_folder_path = "";
	
	if(argc > 1){
		file += argv[1];
		
		if(argc > 2){
			output_folder_path += argv[2];
		}
	}
	
	if (Loader::DetectFileType(file) == Loader::FileTypes::Pkg) {
		std::cout << file << " is a valid PKG\n" << std::endl;
		
		PKG pkg = PKG();
		
		std::string failreason;
		if (!pkg.Open(file, failreason)) {
            std::cout << "Cannot open PKG file : " << failreason << std::endl;
        }else{
			std::cout << "open PKG file success" << std::endl;
			
			PSF psf = PSF();
			
			if (!psf.Open(pkg.sfo)) {
				std::cout << "Could not read SFO." << std::endl;
			}else{
				std::cout << "open PSF success" << std::endl;
				
				output_folder_path /= pkg.GetTitleID();
				
				std::string category;
				category += *psf.GetString("CATEGORY");
				std::cout << "PSF category = " << category << std::endl;
				
				std::string pkgType = pkg.GetPkgFlags();
				std::cout << "pkgType = " << pkgType << std::endl;
				
				if(pkgType.contains("PATCH")){
					std::cout << "pkg is a game update" << std::endl;
					output_folder_path += "-patch";
				}else if(category == "ac"){
					std::cout << "pkg is a dlc" << std::endl;
					std::string content_id;
					if (auto value = psf.GetString("CONTENT_ID"); value.has_value()) {
						content_id = std::string{*value};
						std::string entitlement_label = Common::SplitString(content_id, '-')[2];
						output_folder_path /= entitlement_label;
					} else {
						std::cout << "PSF file there is no CONTENT_ID" << std::endl;
					}
				}else{
					std::cout << "pkg is a base game" << std::endl;
				}
				
				std::cout << "Extracting pkg to " << output_folder_path << std::endl;
				
				if (!pkg.Extract(file, output_folder_path, failreason)) {
					std::cout << "Cannot extract PKG file : " << failreason << std::endl;
				} else {
					int nfiles = pkg.GetNumberOfFiles();
					
					for(int i=0; i<nfiles; i++)
					{
						std::cout << "Extracting file " << i+1 << " of " << nfiles << " to " << output_folder_path << std::endl;
						pkg.ExtractFiles(i);
					}
				}
			}
		}
	} else {
		std::cout << file << " doesn't appear to be a valid PKG file" << std::endl;
	}
	
	std::cout << "\nTHE END " << file << std::endl;
	
	if(argc == 2){
		std::cout << "\nPress [enter] to exit." << std::endl;
		std::cin.get();
	}
	
	return 0;
}
