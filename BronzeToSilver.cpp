#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <cmath>
 
using namespace std;
 
/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
 
struct Point
{
    int x = 0;
    int y = 0;
 
    bool operator ==(const Point& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
 
    bool operator !=(const Point& rhs) const
    {
        return x != rhs.x || y != rhs.y;
    }
};
 
namespace MathHelpers
{
    float SquareDistance(const Point& Position1, const Point& Position2) {
        return (Position1.x - Position2.x) * (Position1.x - Position2.x) +
               (Position1.y - Position2.y) * (Position1.y - Position2.y);
    }
 
    float Distance(const Point& Position1, const Point& Position2) {
        return sqrt(SquareDistance(Position1, Position2));
    }
 
    float VectorLenght(const Point& Position) {
        return sqrt((Position.x * Position.x) + (Position.y * Position.y));
    }
};
 
#define LOGGER 0
 
namespace Logger
{
    void LogError(const string& message)
    {
#if LOGGER
        cerr << message << endl;
#endif
    }
};
 
class Pod
{
public:
    Pod(const Point& InPosition) : position(InPosition) {}
 
    void SetPosition(const Point& InPosition);
    void SetAngle(const int InAngle);
    void SetSpeed(const Point& InSpeed);
    void UpdatePod(const Point& InCoord, const int InAngle);
    void UpdateNextCheckpoint(const Point& InNextCheckPoint);
 
    const int radius = 400;
    Point position = Point();
    int angle = 0;
    bool drifting = false;
    int used_boosts = 0;
    Point speed = Point();
    Point next_checkpoint = Point();
};
 
inline void Pod::SetPosition(const Point& InPosition)
{
    position = InPosition;
}
 
inline void Pod::SetAngle(const int InAngle)
{
    angle = InAngle;
}
 
inline void Pod::SetSpeed(const Point& InSpeed)
{
    speed = InSpeed;
}
 
inline void Pod::UpdatePod(const Point& InCoord, const int InAngle)
{
    position = InCoord;
    speed = InCoord;
    angle = InAngle;
}
 
inline void Pod::UpdateNextCheckpoint(const Point& InNextCheckPoint)
{
    if(next_checkpoint != InNextCheckPoint)
    {
        drifting = false;
        next_checkpoint = InNextCheckPoint;
    }
}
 
class RaceCircuit
{
public:
    RaceCircuit() {}
    ~RaceCircuit()
    {
        delete PlayerPod;
    }
 
    void UpdateCheckpoints(const Point& InCheckpoint);
    void UpdateCheckpointDistance(const int InDistance);
 
    bool IsNewCheckPoint(const Point& InCheckpoint);
    int GetCheckpointIndex(const Point& InCheckpoint);
 
    void CalcBoostMoment();
 
    string CalculateThrust();
 
    void UpdatePod();
 
    void Update();
 
    Pod* PlayerPod = new Pod({0,0});
 
    vector<Point> CheckPoints = vector<Point>();
 
    Point next_checkpoint = Point();
    Point boost_moment = Point();
    int next_checkpoint_distance = 0;
    int checkpoint_num = 0;
    bool checkpoints_in_list = false;
};
 
bool RaceCircuit::IsNewCheckPoint(const Point& InCheckpoint)
{
    for(const Point& checkpoint : CheckPoints)
    {
        if(checkpoint == InCheckpoint)
        {
            Logger::LogError("IsNewCheckPoint true");
            return false;
        }
    }
 
    Logger::LogError("IsNewCheckPoint false");
    return true;
}
 
int RaceCircuit::GetCheckpointIndex(const Point& InCheckpoint)
{
    const int size = CheckPoints.size();
    for(int Idx = 0; Idx < size - 1; ++Idx)
    {
        if(CheckPoints[Idx] == InCheckpoint)
        {
            Logger::LogError("GetCheckpointIndex" + to_string(Idx));
            return Idx;
        }
    }
 
    Logger::LogError("GetCheckpointIndex false");
    return -1;
}
 
void RaceCircuit::UpdateCheckpoints(const Point& InCheckpoint)
{
    if(IsNewCheckPoint(InCheckpoint) == true && InCheckpoint != next_checkpoint)
    {
        CheckPoints.emplace_back(InCheckpoint);
        checkpoint_num = CheckPoints.size();
        next_checkpoint = InCheckpoint;
        Logger::LogError("NewCheckpoint");
    }
    else if(InCheckpoint != next_checkpoint && checkpoints_in_list == false)
    {
        checkpoints_in_list = true;
        checkpoint_num = CheckPoints.size();
        CalcBoostMoment();
        Logger::LogError("BoostMovement");
    }
    else
    {
        next_checkpoint = InCheckpoint;
        Logger::LogError("NextCheckpoint");
    }
}
 
void RaceCircuit::UpdateCheckpointDistance(const int InDistance)
{
    next_checkpoint_distance = InDistance;
}
 
void RaceCircuit::CalcBoostMoment()
{
    if(checkpoint_num == 0)
    {
        Logger::LogError("CalcBoostMoment() : checkpoint_num");
        return;
    }
 
    map<int, int> distancesMap;
    vector<int> distances;
    const int Size = CheckPoints.size();
 
    for(int i = 0; i < Size - 1; ++i)
    {
        distances.emplace_back(MathHelpers::Distance(CheckPoints[i], CheckPoints[(i+1) % checkpoint_num]));
        distancesMap.emplace(MathHelpers::Distance(CheckPoints[i], CheckPoints[(i+1) % checkpoint_num]), i);
    }
 
    sort(distances.begin(), distances.end(), greater<int>());
 
    if(distances.size() > 0 && distancesMap.size() > 0)
    {
        boost_moment = CheckPoints[distancesMap[distances[0]]];
    }
}
 
string RaceCircuit::CalculateThrust()
{
    int thrust = 100;
 
    if(PlayerPod == nullptr)
    {
        Logger::LogError("CalculateThrust() : PlayerPod");
        return to_string(thrust);
    }
 
    constexpr int MaxTurnAngle = 18;
    if(PlayerPod->angle > -MaxTurnAngle && PlayerPod->angle < MaxTurnAngle)
    {
        if(next_checkpoint == boost_moment)
        {
            return " BOOST";
        }
    }
    else
    {
        thrust *= max(min((1 - abs(PlayerPod->angle) / 90), 1), 0);
    }
 
    constexpr int CheckpointRadius = 400;
    if(checkpoints_in_list == true)
    {
        thrust *= max(min(next_checkpoint_distance / (CheckpointRadius * 2), 1), 0);
    }
    else
    {
        thrust *= max(min(next_checkpoint_distance / (CheckpointRadius * 3), 1), 0);
    }
 
    return to_string(thrust);
}
 
void RaceCircuit::Update()
{
    if(checkpoint_num == 0 || PlayerPod == nullptr)
    {
        Logger::LogError("Update() : checkpoint_num or PlayerPod");
        return;
    }
 
    const string thrust = " " + CalculateThrust();
 
    constexpr int DriftAngle = 6;
    if(MathHelpers::VectorLenght(PlayerPod->speed) * 3 > next_checkpoint_distance
        && checkpoints_in_list == true
        && ((PlayerPod->angle > -DriftAngle && PlayerPod->angle < DriftAngle) || PlayerPod->drifting == true))
    {
        const int index = GetCheckpointIndex(next_checkpoint);
        if(index == -1)
        {
            Logger::LogError("Update() : Fail index");
            return;
        }
 
        const Point next_position = CheckPoints[(index + 1) % checkpoint_num];
        PlayerPod->drifting = true;
       
		constexpr int minimal_drift_speed = 60;
        if(minimal_drift_speed > MathHelpers::VectorLenght(PlayerPod->speed))
        {
            PlayerPod->drifting = false;
        }

		constexpr int drift_speed = 40;
        cout << next_position.x << " " << next_position.y << " " << drift_speed << endl;
    }
    else
    {
        cout << next_checkpoint.x << " " << next_checkpoint.y << thrust << endl;
    }
}
 
int main()
{
 
    RaceCircuit* circuit = new RaceCircuit();
    // game loop
    while (1) {
        int x;
        int y;
        int next_checkpoint_x; // x position of the next check point
        int next_checkpoint_y; // y position of the next check point
        int next_checkpoint_dist; // distance to the next checkpoint
        int next_checkpoint_angle; // angle between your pod orientation and the direction of the next checkpoint
        cin >> x >> y >> next_checkpoint_x >> next_checkpoint_y >> next_checkpoint_dist >> next_checkpoint_angle; cin.ignore();
        int opponent_x;
        int opponent_y;
        cin >> opponent_x >> opponent_y; cin.ignore();
 
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
 
        // You have to output the target position
        // followed by the power (0 <= thrust <= 100) or "BOOST"
        // i.e.: "x y thrust"
        //cout << next_checkpoint_x << " " << next_checkpoint_y << " " << 100 << endl;
 
        if(circuit != nullptr)
        {
            if(circuit->PlayerPod != nullptr)
            {
                circuit->PlayerPod->UpdatePod({x,y}, next_checkpoint_angle);
                circuit->PlayerPod->UpdateNextCheckpoint({next_checkpoint_x, next_checkpoint_y});
            }
 
            circuit->UpdateCheckpoints(Point({next_checkpoint_x, next_checkpoint_y}));
            circuit->UpdateCheckpointDistance(next_checkpoint_dist);
 
            circuit->Update();
        }
    }
 
    delete circuit;
}
