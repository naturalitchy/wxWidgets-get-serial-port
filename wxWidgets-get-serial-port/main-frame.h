#ifndef main_frame_h
#define main_frame_h

#include <wx/wx.h>
#include <wx/tglbtn.h>			// for wxToggleButton.
//#include "get-serial-port.h"
#include "static-variable.h"


class MainFrame : public wxFrame {
	private:

	public:
		MainFrame();
		MainFrame(const wxString &title);
		void ClickedButton(wxCommandEvent &evt);
		int getCOMPort();
};

#endif