// Copyright (c) 2023 OPEN CASCADE SAS
// 
// This file is part of commercial software by OPEN CASCADE SAS, 
// furnished in accordance with the terms and conditions of the contract 
// and with the inclusion of this copyright notice. 
// This file or any part thereof may not be provided or otherwise 
// made available to any third party. 
// 
// No ownership title to the software is transferred hereby. 
// 
// OPEN CASCADE SAS makes no representation or warranties with respect to the 
// performance of this software, and specifically disclaims any responsibility 
// for any damages, special or consequential, connected with its use.

#ifndef _TKXDEUSD_HeaderFile
#define _TKXDEUSD_HeaderFile

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>

//! Interface to activate license for TKDEUSD toolkit
//! Activation is recursive process, all depends toolkit will be activated too
//! Activation only affects the current session within a single process
//! Packages:
//! - USD
class TKDEUSD
{
public:

  //! Activates the application license for a current product by a key string or license path or server.
  //! The call should be made prior to usage of the corresponding product.
  //! Only the one call name will be accepted.
  //! @param[in] theLicense license key or path to license file or license server
  Standard_EXPORT static void Activate(const TCollection_AsciiString& theLicense);

  //! Returns host ID as unsigned long. For Windows network adapter number can be specified.
  //! @param[in] theNetworkAdapterNum used only on Windows machines to get host ID by specified network adapter number.
  Standard_EXPORT static TCollection_AsciiString GetHostID(const int theNetworkAdapterNum = 0);

  //! Returns host ID as char array and license info.
  //! @param[out] theHostId host ID as string
  //! @param[out] theLicensesInfo info about available licenses.
  Standard_EXPORT static void GetLicenseInfo(TCollection_AsciiString& theHostId,
                                             TCollection_AsciiString& theLicensesInfo);

  //! Loads license keys from a specified file or server.
  //! After a successful load, the current toolkit is activated and all its dependencies too
  //! In case of license file or server activates toolkits with it as a global license server or file
  //! The keys or server or file should be loaded before any calls to corresponding products.
  //! Consecutive calls are possible, allowing to load several files.
  //! Returns True if file is found, else False.
  //! Note: that this function does not check the license(s).
  //! @param[in] thePath path to license file or license server
  //! @param[in] theIsVerbose flag to print info
  Standard_EXPORT static bool LoadLicenseFileOrServer(const TCollection_AsciiString& thePath,
                                                      const bool theIsVerbose);

  //! Sets global license prefix for all products
  //! @param[in] thePrefix license prefix, which is prepended to each feature name during license checking out
  Standard_EXPORT static void SetGlobalLicensePrefix(const TCollection_AsciiString& thePrefix);

};

#endif // _TKXDEUSD_HeaderFile
