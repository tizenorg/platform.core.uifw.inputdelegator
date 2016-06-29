Name: org.tizen.inputdelegator
Summary: Input Delegator Application
Version: 0.0.160629
Release: 1
Group: Applications
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz

%define enable_log_manager 1

BuildRequires:  pkgconfig(ecore-wayland)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(edbus)

BuildRequires: cmake
BuildRequires: edje-bin
BuildRequires: gettext

BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-media-wav-player)
BuildRequires: pkgconfig(capi-system-device)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(efl-assist)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(feedback)
BuildRequires: pkgconfig(stt)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(db-util)
BuildRequires: pkgconfig(sqlite3)
#BuildRequires: pkgconfig(smartreply)


%if %{enable_log_manager}
BuildRequires:  pkgconfig(bundle)
%endif


%define _appdir		/usr/apps
%define _app_destdir	%{_appdir}/%{name}

# Sandboxed prefix '/'
%define _app_prefix	/
%define _app_localedir %{_app_prefix}res/locale

%description
Input Delegator Application for Wearable environment

%prep
%setup -q

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

export CXXFLAGS="$CXXFLAGS -std=gnu++0x"


cmake . -DCMAKE_INSTALL_PREFIX=%{_app_prefix} \
	-DCMAKE_INSTALL_LOCALEDIR=%{_app_localedir} \
	-DENABLE_LOG_MANAGER=%{enable_log_manager}

# compile warning & error filtering
make %{?jobs:-j%jobs} \
  2>&1 | sed \
  -e 's%^.*: error: .*$%\x1b[1;31;40m&\x1b[m%' \
  -e 's%^.*: warning: .*$%\x1b[1;33;40m&\x1b[m%'

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}%{_app_destdir}

mkdir -p %{buildroot}%{_datadir}/packages/
cp -rf org.tizen.inputdelegator.xml %{buildroot}%{_datadir}/packages

%post
#nothing

/usr/bin/vconftool -u 5000 set -t int db/private/ise/stt/language "0" -f -s tizen::vconf::platform::rw

%files
%manifest org.tizen.inputdelegator.manifest
%defattr(-,app,app,-)

%{_datadir}/packages/org.tizen.inputdelegator.xml
%{_app_destdir}%{_app_prefix}/bin/inputdelegator
%{_app_destdir}%{_app_prefix}/res/edje/w-input-selector.edj
%{_app_destdir}%{_app_prefix}/res/edje/w-input-stt.edj
%{_app_destdir}%{_app_prefix}/res/edje/w-input-stt-button.edj
%{_app_destdir}%{_app_prefix}/res/images/*
%{_app_destdir}%{_app_prefix}/shared/res/w-input-selector.png
%{_app_destdir}%{_app_localedir}/*/LC_MESSAGES/org.tizen.inputdelegator.mo
