Option Explicit

Dim f As wxFrame Ptr

Sub OnButtonClick(ByRef ev As wxCommandEvent)

	Dim entry As wxZipEntry Ptr
	
	Dim in As wxFFileInputStream("OOP.zip")
	Dim zip As wxZipInputStream(in)

	entry = zip.GetNextEntry()
	Do While entry <> Nothing
		MsgBox entry->GetInternalName()
		
		wxDELETE( entry )
		entry = zip.GetNextEntry()
	Loop
	wxDELETE( entry )
End Sub

Sub Main

	f = New wxFrame( NULL, wxID_ANY, "Zip Sample" )
	f.SetClientSize(wxSize(300,100))
	
	Dim p As New wxPanel(f, wxID_ANY)
	Dim btnTest As New wxButton(p, 100, "Read zip", wxPoint(100,20))
	btnTest.Bind( wxEVT_BUTTON, Addressof OnButtonClick )

	f.Show(TRUE)
End Sub

