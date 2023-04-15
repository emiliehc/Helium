using System.Runtime.CompilerServices;

namespace Helium;

[NativelyMapped("src/Helium/CoreGame/ActorComponent.h")]
public class ActorComponent : Object
{
    protected ActorComponent() { }
    
    public virtual void Awake() { }
    public virtual void Start() { }
    public virtual void OnUpdate() { }
    public virtual void OnFixedUpdate() { }
    public virtual void OnDestroy() { }
}