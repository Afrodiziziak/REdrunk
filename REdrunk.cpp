#include <Windows.h>
#include <iostream>
#include <conio.h>


struct ConsoleWrapper_t {
	enum struct ArrowsCode_t : char { None = 255, Up = 72, Down = 80, Left = 75, Right = 77 };

	static constexpr const uint32_t DisplayWidth	= 50;
	static constexpr const uint32_t DisplayHeight	= 25;

	struct ContextWrapper_t {
		ContextWrapper_t() { Fill(' '); }

		template <typename T>
		void SetPosition(T object) { Data[object.GetY()][object.GetX()] = object.GetIMG(); }

		void Fill(char symbol) {
			for (auto x = 0; x < DisplayHeight; x++) {
				for (auto y = 0; y < DisplayWidth; y++) {
					Data[x][y] = symbol;
				}
			}
		}

		void Print() {
			for (auto x = 0; x < DisplayHeight; x++) {
				for (auto y = 0; y < DisplayWidth; y++) {
					std::cout << Data[x][y];
				}
				std::cout << std::endl;
			}
			Fill(' ');
		}

		char GetElement(int y, int x) {	return Data[y][x]; }

		char Data[DisplayHeight][DisplayWidth];
	};

	ConsoleWrapper_t() {
		mStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		system("cls");
		__ConsoleCursorVisible(false);
	}

	void SwapBuffers() {
		SetConsoleCursorPosition(mStdOut, { 0, 0 });
		mContext.Print();
	}

	ArrowsCode_t GetArrowsCode() {
		char input = __GetCharWithoutBlocking();
		if (input == -32) input = __GetCharWithoutBlocking();
		return static_cast<ArrowsCode_t>(input);
	}

	ContextWrapper_t& Context() { return mContext; }

private:
	void __ConsoleCursorVisible(bool show) {
		CONSOLE_CURSOR_INFO structCursorInfo;
		GetConsoleCursorInfo(mStdOut, &structCursorInfo);
		structCursorInfo.bVisible = show;
		SetConsoleCursorInfo(mStdOut, &structCursorInfo);
	}

	int __GetCharWithoutBlocking() {
		if (_kbhit()) return _getch();
		else return -1;
	}

	HANDLE				mStdOut;
	ContextWrapper_t	mContext;
};

struct Model_t {
	int& GetX()		{ return mX; }
	int& GetY()		{ return mY; }
	char GetIMG()	{ return mImg; }

protected:
	int		mX, mY;
	char	mImg;
};

struct Player_t : public Model_t {
	enum class CardinalDirections_t { N, S, W, E, COUNT };

	Player_t() {
		mX		= ConsoleWrapper_t::DisplayWidth / 2;
		mY		= ConsoleWrapper_t::DisplayHeight / 2;
		mImg	= '@';

		Fill();
	}

	void Collider(ConsoleWrapper_t& object) {
		int x = mX;
		int y = mY;

		if (mY == 0 || object.Context().GetElement(--y, x) != ' ')
			Obstacles[(int)CardinalDirections_t::N] = false;
		else {
			Obstacles[(int)CardinalDirections_t::N] = true;
			++y;
		}

		if (mY == ConsoleWrapper_t::DisplayHeight - 1 || object.Context().GetElement(++y, x) != ' ')
			Obstacles[(int)CardinalDirections_t::S] = false;
		else {
			Obstacles[(int)CardinalDirections_t::S] = true;
			--y;
		}

		if (mX == 0 || object.Context().GetElement(y, --x) != ' ')
			Obstacles[(int)CardinalDirections_t::W] = false;
		else {
			Obstacles[(int)CardinalDirections_t::W] = true;
			++x;
		}

		if (mX == ConsoleWrapper_t::DisplayWidth - 1 || object.Context().GetElement(y, ++x) != ' ')
			Obstacles[(int)CardinalDirections_t::E] = false;
		else {
			Obstacles[(int)CardinalDirections_t::E] = true;
			--x;
		}
	}

	bool GetN() { return Obstacles[(int)CardinalDirections_t::N]; }
	bool GetS() { return Obstacles[(int)CardinalDirections_t::S]; }
	bool GetW() { return Obstacles[(int)CardinalDirections_t::W]; }
	bool GetE() { return Obstacles[(int)CardinalDirections_t::E]; }

private:
	void Fill() { for (int i = 0; i < SIZE; i++) { Obstacles[i] = true; } }

	static const int SIZE = static_cast<int>(CardinalDirections_t::COUNT);
	bool Obstacles[SIZE]{};
};

struct NPC_t : public Model_t {
	NPC_t() {
		mX		= 12;
		mY		= 12;
		mImg	= '*';
	}
};

int main()
{
	Player_t Player;
	NPC_t npc;
	ConsoleWrapper_t console;

	while (true)
	{
		auto arrowCode = console.GetArrowsCode();

		if (arrowCode != decltype(arrowCode)::None)
		{
			switch (arrowCode)
			{
			case ConsoleWrapper_t::ArrowsCode_t::Up:
				if (Player.GetN())	Player.GetY()--;
				break;

			case ConsoleWrapper_t::ArrowsCode_t::Down:
				if (Player.GetS())	Player.GetY()++;
				break;

			case ConsoleWrapper_t::ArrowsCode_t::Left:
				if (Player.GetW())	Player.GetX()--;
				break;

			case ConsoleWrapper_t::ArrowsCode_t::Right:
				if (Player.GetE())	Player.GetX()++;
				break;
			}
		}
		Player.Collider(console);
		console.Context().SetPosition(Player);
		console.SwapBuffers();
		console.Context().SetPosition(npc);
	}

	return 0;
}