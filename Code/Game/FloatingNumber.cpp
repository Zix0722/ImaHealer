#include "FloatingNumber.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Easing.hpp"

FloatingNumber::FloatingNumber(Vec2 start, float num, FloatingNumType type)
	:m_startPos(start)
	,m_number(num)
{
	m_movingDegrees = g_theRNG->RollRandomFloatInRange(0.f, 60.f);
	m_type = type;
	m_stopWatch = new Stopwatch(1.f);
	m_stopWatch->Start();
	float size = RangeMapClamped(num, 0.f, 1000.f, 200.f, 500.f);
	m_bounds.SetDimensions(Vec2(size, size));
}

FloatingNumber::~FloatingNumber()
{

}

void FloatingNumber::Update(float deltaseconds)
{
	UNUSED(deltaseconds);
	if (m_stopWatch->HasDurationElapsed())
	{
		m_isEnd = true;
	}
}

void FloatingNumber::Render() const
{
	float movingDist = 100.f;
	AABB2 drawBox = m_bounds;
	Vec2 pos = m_startPos;
	unsigned char Alpha = 255;
	Rgba8 color;
	if (m_type == FloatingNumType::Damage)
	{
		color = Rgba8::RED;
	}
	else
	{
		color = Rgba8::GREEN;
	}

	if (!m_stopWatch->HasDurationElapsed())
	{
		if (m_stopWatch->GetElapsedFration() < 0.1f)
		{
			float fraction = RangeMapClamped(m_stopWatch->GetElapsedFration(), 0.f, 0.1f, 0.f, 1.f);
			drawBox.SetDimensions(Vec2(m_bounds.GetDimensions().x * fraction, m_bounds.GetDimensions().y * fraction));

		}
		else
		{
			float fraction = RangeMapClamped(m_stopWatch->GetElapsedFration(), 0.2f, 1.f, 0.f, 1.f);
			
			fraction = SmoothStop2(fraction);
			Vec2 translation = Vec2::MakeFromPolarDegrees(m_movingDegrees, movingDist * fraction);
			TransformPosition2D(pos, 1.f, 0.f, translation);
			
			Alpha = (unsigned char)RangeMapClamped(m_stopWatch->GetElapsedFration(), 0.2f, 1.0f, 255.f, 50.f);
			
		}
		drawBox.SetCenter(pos);

		g_theRenderer->SetBlendMode(BlendMode::ALPHA);

		std::vector<Vertex_PCU> txtVerts;
		std::string str;
		if (m_type == FloatingNumType::Damage)
		{
			str = Stringf("- %.f", fabsf(m_number));
		}
		else
		{
			str = Stringf("+ %.f", m_number);
		}
		color.a = Alpha;
		m_gameFont->AddVertsForTextInBox2D(txtVerts, drawBox, 15.f, str, color, 1.2f);
		g_theRenderer->BindTexture(&m_gameFont->GetTexture());
		g_theRenderer->DrawVertexArray((int)txtVerts.size(), txtVerts.data());


	}
}

FloatingNumber* FloatingNumber::SpawnNumber(float num, FloatingNumType type, Vec2 pos)
{
	return new FloatingNumber(pos, num, type);
}

