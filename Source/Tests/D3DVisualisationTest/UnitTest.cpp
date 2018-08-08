#include "stdafx.h"
#include <Windows.h>
#include "D3DVisualization.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;

namespace D3DVisualisationTest
{
	[TestClass()]
	public ref class UnitTest
	{
	private:
		TestContext^ testContextInstance;

	public: 
		/// <summary>
		///Gets or sets the test context which provides
		///information about and functionality for the current test run.
		///</summary>
		property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext
		{
			Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
			{
				return testContextInstance;
			}
			System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
			{
				testContextInstance = value;
			}
		};

		#pragma region Additional test attributes
		//
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		[ClassInitialize()]
		static void MyClassInitialize(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ /*context*/)
		{
			Assert::AreEqual<HRESULT>(S_OK, Init());
		};
		//
		//Use ClassCleanup to run code after all tests in a class have run
		[ClassCleanup()]
		static void MyClassCleanup() 
		{
			Assert::AreEqual<HRESULT>(S_OK, Cleanup());
		};
		
		//Use TestInitialize to run code before running each test
		//[TestInitialize()]
		//void MyTestInitialize() {};
		//
		//Use TestCleanup to run code after each test has run
		//[TestCleanup()]
		//void MyTestCleanup() 
		//{
		//};
		
		#pragma endregion 

		[TestMethod]
		void TestRender()
		{
			Assert::AreEqual<HRESULT>(S_OK, Render(nullptr, false));
		};
	};
}
