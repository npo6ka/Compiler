#pragma once



namespace lol {
    int _countImages = 0;

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// —водка дл€ MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
    private: System::Windows::Forms::ImageList^  imageList1;
    private: System::Windows::Forms::PictureBox^  pictureBox1;
    private: System::Windows::Forms::Button^  button1;
    private: System::Windows::Forms::Button^  button2;



    protected: 
    private: System::ComponentModel::IContainer^  components;

    protected: 



	private:
		/// <summary>
		/// “ребуетс€ переменна€ конструктора.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// ќб€зательный метод дл€ поддержки конструктора - не измен€йте
		/// содержимое данного метода при помощи редактора кода.
		/// </summary>
		void InitializeComponent(void)
        {
            this->components = (gcnew System::ComponentModel::Container());
            System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
            this->imageList1 = (gcnew System::Windows::Forms::ImageList(this->components));
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->button2 = (gcnew System::Windows::Forms::Button());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
            this->SuspendLayout();
            // 
            // imageList1
            // 
            this->imageList1->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"imageList1.ImageStream")));
            this->imageList1->TransparentColor = System::Drawing::Color::DimGray;
            this->imageList1->Images->SetKeyName(0, L"Ship_4.png");
            this->imageList1->Images->SetKeyName(1, L"Ship_3.png");
            this->imageList1->Images->SetKeyName(2, L"Ship_2.png");
            this->imageList1->Images->SetKeyName(3, L"Ship_1.png");
            // 
            // pictureBox1
            // 
            this->pictureBox1->Location = System::Drawing::Point(136, 131);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(128, 128);
            this->pictureBox1->TabIndex = 0;
            this->pictureBox1->TabStop = false;
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(189, 355);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(75, 23);
            this->button1->TabIndex = 1;
            this->button1->Text = L"<<";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
            // 
            // button2
            // 
            this->button2->Location = System::Drawing::Point(313, 355);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(75, 23);
            this->button2->TabIndex = 2;
            this->button2->Text = L">>";
            this->button2->UseVisualStyleBackColor = true;
            this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
            // 
            // MyForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(960, 480);
            this->Controls->Add(this->button2);
            this->Controls->Add(this->button1);
            this->Controls->Add(this->pictureBox1);
            this->Name = L"MyForm";
            this->Text = L"MyBattleShip";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion

private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
             if (++_countImages >= 4) _countImages = 0;
             this->pictureBox1->Size = this->imageList1->Images[_countImages]->Size;
             this->pictureBox1->Image = this->imageList1->Images[_countImages];
         }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
             if (--_countImages < 0) _countImages = 3;
             this->pictureBox1->Image = this->imageList1->Images[_countImages];
         }


private: System::Void progressBar1_Click(System::Object^  sender, System::EventArgs^  e) {
             
         }
};
}
