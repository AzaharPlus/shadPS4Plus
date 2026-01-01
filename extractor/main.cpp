// adapted from InstallDragDropPkg in main_window.cpp

#include <iostream>
#include "core/file_format/pkg.h"
#include "core/file_format/psf.h"
#include "common/path_util.h"
#include "common/string_util.h"
#include "core/loader.h"

int main(int argc, char** argv){	
	std::filesystem::path file = "D:\\test.pkg";
	std::filesystem::path output_folder_path = "D:\\install";
	
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
	return 0;
	
	// pkg viewer
	/*	PKGViewer* pkgViewer = new PKGViewer(
			m_game_info, this, [this](std::filesystem::path file, int pkgNum, int nPkg) {
				this->InstallDragDropPkg(file, pkgNum, nPkg);
			});
		pkgViewer->show();
	*/
	
	
	// install one pkg
	/*std::filesystem::path file;
	
    if (Loader::DetectFileType(file) == Loader::FileTypes::Pkg) {
        std::string failreason;
        pkg = PKG();
        if (!pkg.Open(file, failreason)) {
            QMessageBox::critical(this, tr("PKG ERROR"), QString::fromStdString(failreason));
            return;
        }
        if (!psf.Open(pkg.sfo)) {
            QMessageBox::critical(this, tr("PKG ERROR"),
                                  "Could not read SFO. Check log for details");
            return;
        }
        auto category = psf.GetString("CATEGORY");

        if (!use_for_all_queued || pkgNum == 1) {
            InstallDirSelect ids;
            const auto selected = ids.exec();
            if (selected == QDialog::Rejected) {
                return;
            }

            last_install_dir = ids.getSelectedDirectory();
            delete_file_on_install = ids.deleteFileOnInstall();
            use_for_all_queued = ids.useForAllQueued();
        }
        std::filesystem::path game_install_dir = last_install_dir;

        QString pkgType = QString::fromStdString(pkg.GetPkgFlags());
        bool use_game_update = pkgType.contains("PATCH") && Config::getSeparateUpdateEnabled();

        // Default paths
        auto game_folder_path = game_install_dir / pkg.GetTitleID();
        auto game_update_path = use_game_update ? game_folder_path.parent_path() /
                                                      (std::string{pkg.GetTitleID()} + "-patch")
                                                : game_folder_path;
        const int max_depth = 5;

        if (pkgType.contains("PATCH")) {
            // For patches, try to find the game recursively
            auto found_game = Common::FS::FindGameByID(game_install_dir,
                                                       std::string{pkg.GetTitleID()}, max_depth);
            if (found_game.has_value()) {
                game_folder_path = found_game.value().parent_path();
                game_update_path = use_game_update ? game_folder_path.parent_path() /
                                                         (std::string{pkg.GetTitleID()} + "-patch")
                                                   : game_folder_path;
            }
        } else {
            // For base games, we check if the game is already installed
            auto found_game = Common::FS::FindGameByID(game_install_dir,
                                                       std::string{pkg.GetTitleID()}, max_depth);
            if (found_game.has_value()) {
                game_folder_path = found_game.value().parent_path();
            }
            // If the game is not found, we install it in the game install directory
            else {
                game_folder_path = game_install_dir / pkg.GetTitleID();
            }
            game_update_path = use_game_update ? game_folder_path.parent_path() /
                                                     (std::string{pkg.GetTitleID()} + "-patch")
                                               : game_folder_path;
        }

        QString gameDirPath;
        Common::FS::PathToQString(gameDirPath, game_folder_path);
        QDir game_dir(gameDirPath);
        if (game_dir.exists()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("PKG Extraction"));

            std::string content_id;
            if (auto value = psf.GetString("CONTENT_ID"); value.has_value()) {
                content_id = std::string{*value};
            } else {
                QMessageBox::critical(this, tr("PKG ERROR"), "PSF file there is no CONTENT_ID");
                return;
            }
            std::string entitlement_label = Common::SplitString(content_id, '-')[2];

            auto addon_extract_path =
                Config::getAddonInstallDir() / pkg.GetTitleID() / entitlement_label;
            QString addonDirPath;
            Common::FS::PathToQString(addonDirPath, addon_extract_path);
            QDir addon_dir(addonDirPath);

            if (pkgType.contains("PATCH")) {
                QString pkg_app_version;
                if (auto app_ver = psf.GetString("APP_VER"); app_ver.has_value()) {
                    pkg_app_version = QString::fromStdString(std::string{*app_ver});
                } else {
                    QMessageBox::critical(this, tr("PKG ERROR"), "PSF file there is no APP_VER");
                    return;
                }
                std::filesystem::path sce_folder_path =
                    std::filesystem::exists(game_update_path / "sce_sys" / "param.sfo")
                        ? game_update_path / "sce_sys" / "param.sfo"
                        : game_folder_path / "sce_sys" / "param.sfo";
                psf.Open(sce_folder_path);
                QString game_app_version;
                if (auto app_ver = psf.GetString("APP_VER"); app_ver.has_value()) {
                    game_app_version = QString::fromStdString(std::string{*app_ver});
                } else {
                    QMessageBox::critical(this, tr("PKG ERROR"), "PSF file there is no APP_VER");
                    return;
                }
                double appD = game_app_version.toDouble();
                double pkgD = pkg_app_version.toDouble();
                if (pkgD == appD) {
                    msgBox.setText(QString(tr("Patch detected!") + "\n" +
                                           tr("PKG and Game versions match: ") + pkg_app_version +
                                           "\n" + tr("Would you like to overwrite?")));
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::No);
                } else if (pkgD < appD) {
                    msgBox.setText(QString(tr("Patch detected!") + "\n" +
                                           tr("PKG Version %1 is older than installed version: ")
                                               .arg(pkg_app_version) +
                                           game_app_version + "\n" +
                                           tr("Would you like to overwrite?")));
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::No);
                } else {
                    msgBox.setText(QString(tr("Patch detected!") + "\n" +
                                           tr("Game is installed: ") + game_app_version + "\n" +
                                           tr("Would you like to install Patch: ") +
                                           pkg_app_version + " ?"));
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::No);
                }
                int result = msgBox.exec();
                if (result == QMessageBox::Yes) {
                    // Do nothing.
                } else {
                    return;
                }
            } else if (category == "ac") {
                if (!addon_dir.exists()) {
                    QMessageBox addonMsgBox;
                    addonMsgBox.setWindowTitle(tr("DLC Installation"));
                    addonMsgBox.setText(QString(tr("Would you like to install DLC: %1?"))
                                            .arg(QString::fromStdString(entitlement_label)));

                    addonMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    addonMsgBox.setDefaultButton(QMessageBox::No);
                    int result = addonMsgBox.exec();
                    if (result == QMessageBox::Yes) {
                        game_update_path = addon_extract_path;
                    } else {
                        return;
                    }
                } else {
                    msgBox.setText(QString(tr("DLC already installed:") + "\n" + addonDirPath +
                                           "\n\n" + tr("Would you like to overwrite?")));
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::No);
                    int result = msgBox.exec();
                    if (result == QMessageBox::Yes) {
                        game_update_path = addon_extract_path;
                    } else {
                        return;
                    }
                }
            } else {
                msgBox.setText(QString(tr("Game already installed") + "\n" + gameDirPath + "\n" +
                                       tr("Would you like to overwrite?")));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int result = msgBox.exec();
                if (result == QMessageBox::Yes) {
                    // Do nothing.
                } else {
                    return;
                }
            }
        } else {
            // Do nothing;
            if (pkgType.contains("PATCH") || category == "ac") {
                QMessageBox::information(
                    this, tr("PKG Extraction"),
                    tr("PKG is a patch or DLC, please install the game first!"));
                return;
            }
            // what else?
        }
        if (!pkg.Extract(file, game_update_path, failreason)) {
            QMessageBox::critical(this, tr("PKG ERROR"), QString::fromStdString(failreason));
        } else {
            int nfiles = pkg.GetNumberOfFiles();

            if (nfiles > 0) {
                QVector<int> indices;
                for (int i = 0; i < nfiles; i++) {
                    indices.append(i);
                }

                QProgressDialog dialog;
                dialog.setWindowTitle(tr("PKG Extraction"));
                dialog.setWindowModality(Qt::WindowModal);
                QString extractmsg = QString(tr("Extracting PKG %1/%2")).arg(pkgNum).arg(nPkg);
                dialog.setLabelText(extractmsg);
                dialog.setAutoClose(true);
                dialog.setRange(0, nfiles);

                dialog.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                                       dialog.size(), this->geometry()));

                QFutureWatcher<void> futureWatcher;
                connect(&futureWatcher, &QFutureWatcher<void>::finished, this, [=, this]() {
                    if (pkgNum == nPkg) {
                        QString path;

                        // We want to show the parent path instead of the full path
                        Common::FS::PathToQString(path, game_folder_path.parent_path());
                        QIcon windowIcon(
                            Common::FS::PathToUTF8String(game_folder_path / "sce_sys/icon0.png")
                                .c_str());

                        QMessageBox extractMsgBox(this);
                        extractMsgBox.setWindowTitle(tr("Extraction Finished"));
                        if (!windowIcon.isNull()) {
                            extractMsgBox.setWindowIcon(windowIcon);
                        }
                        extractMsgBox.setText(
                            QString(tr("Game successfully installed at %1")).arg(path));
                        extractMsgBox.addButton(QMessageBox::Ok);
                        extractMsgBox.setDefaultButton(QMessageBox::Ok);
                        connect(&extractMsgBox, &QMessageBox::buttonClicked, this,
                                [&](QAbstractButton* button) {
                                    if (extractMsgBox.button(QMessageBox::Ok) == button) {
                                        extractMsgBox.close();
                                        emit ExtractionFinished();
                                    }
                                });
                        extractMsgBox.exec();
                    }
                    if (delete_file_on_install) {
                        std::filesystem::remove(file);
                    }
                });
                connect(&dialog, &QProgressDialog::canceled, [&]() { futureWatcher.cancel(); });
                connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged, &dialog,
                        &QProgressDialog::setValue);
                futureWatcher.setFuture(
                    QtConcurrent::map(indices, [&](int index) { pkg.ExtractFiles(index); }));
                dialog.exec();
            }
        }
    } else {
        QMessageBox::critical(this, tr("PKG ERROR"),
                              tr("File doesn't appear to be a valid PKG file"));
    }
	*/
}
