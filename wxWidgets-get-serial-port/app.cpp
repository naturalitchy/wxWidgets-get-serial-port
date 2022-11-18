#include "app.h"

bool App::OnInit() {
	MainFrame *mainFrame = new MainFrame(" Station V ");
	mainFrame->SetSize(1360, 755);
	mainFrame->Center();

	// Setting the title icon. ico => TYPE_ICO.
	wxIcon icon(wxT("../rect.ico"), wxBITMAP_TYPE_ICO);

	mainFrame->SetIcon(icon);

	mainFrame->Show();

	return true;
}